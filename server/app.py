import csv
import os
import subprocess
import tempfile
from pathlib import Path
from typing import List, Dict, Tuple

from flask import Flask, request

ROOT_DIR = Path(__file__).resolve().parent.parent
WEB_DIR = ROOT_DIR / "web"
DEFAULT_PORT = int(os.environ.get("PORT", "8080"))
app = Flask(__name__, static_folder=str(WEB_DIR), static_url_path="")
app.config["MAX_CONTENT_LENGTH"] = 5 * 1024 * 1024  # 5 MB uploads


def _binary_candidates() -> List[Path]:
    exe_name = "flower_exchange.exe" if os.name == "nt" else "flower_exchange"
    build_dir = ROOT_DIR / "build"
    candidates = [build_dir / exe_name]
    for config in ("Debug", "Release"):
        candidates.append(build_dir / config / exe_name)
    return candidates


def _resolve_binary() -> Path:
    override = os.environ.get("FLOWER_EXCHANGE_BIN")
    if override:
        candidate = Path(override).expanduser()
        if candidate.is_file():
            return candidate
        raise FileNotFoundError(f"FLOWER_EXCHANGE_BIN points to a missing file: {candidate}")

    for candidate in _binary_candidates():
        if candidate.is_file():
            return candidate

    raise FileNotFoundError(
        "Unable to locate the compiled flower_exchange binary. Build the project or set FLOWER_EXCHANGE_BIN."
    )


def _normalize_status(value: str) -> str:
    lowered = value.lower()
    mapping = {
        "pfill": "PFill",
        "fill": "Fill",
        "new": "New",
        "rejected": "Rejected",
    }
    return mapping.get(lowered, value)


def _normalize_side(value: str) -> str:
    lowered = value.lower()
    if lowered == "sell":
        return "Sell"
    if lowered == "buy":
        return "Buy"
    return value or "Unknown"


def _safe_int(value: str) -> int:
    try:
        return int(value)
    except (TypeError, ValueError):
        return 0


def _safe_float(value: str) -> float:
    try:
        return float(value)
    except (TypeError, ValueError):
        return 0.0


def _parse_reports(csv_path: Path) -> List[Dict[str, str]]:
    reports: List[Dict[str, str]] = []
    with csv_path.open(newline="", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        if not reader.fieldnames:
            return reports
        for row in reader:
            reports.append(
                {
                    "orderId": row.get("order_id", ""),
                    "clientOrderId": row.get("client_order_id", ""),
                    "instrument": row.get("instrument", ""),
                    "side": _normalize_side(row.get("side", "")),
                    "status": _normalize_status(row.get("status", "")),
                    "quantity": _safe_int(row.get("executed_quantity", "0")),
                    "price": _safe_float(row.get("executed_price", "0")),
                    "reason": row.get("reason", ""),
                    "timestamp": row.get("transaction_time", ""),
                }
            )
    return reports


def _run_engine(input_path: Path, output_path: Path) -> None:
    binary = _resolve_binary()
    completed = subprocess.run(
        [str(binary), str(input_path), str(output_path)],
        capture_output=True,
        text=True,
        check=False,
    )
    if completed.returncode != 0:
        stderr = completed.stderr.strip() or completed.stdout.strip()
        raise RuntimeError(f"flower_exchange exited with {completed.returncode}: {stderr}")


@app.route("/")
def root() -> str:
    return app.send_static_file("index.html")


@app.route("/health")
def health() -> Tuple[Dict[str, str], int]:
    return {"status": "ok"}, 200


@app.route("/api/execute", methods=["POST"])
def execute() -> Tuple[Dict[str, object], int]:
    if "orders" not in request.files:
        return {"error": "Missing CSV upload."}, 400

    upload = request.files["orders"]
    if upload.filename == "":
        return {"error": "Please choose a CSV file."}, 400

    input_tmp = tempfile.NamedTemporaryFile(delete=False, suffix=".csv")
    output_tmp = tempfile.NamedTemporaryFile(delete=False, suffix=".csv")
    input_path = Path(input_tmp.name)
    output_path = Path(output_tmp.name)
    input_tmp.close()
    output_tmp.close()

    try:
        upload.save(input_path)
        _run_engine(input_path, output_path)
        reports = _parse_reports(output_path)
        csv_payload = output_path.read_text(encoding="utf-8")
        return {
            "reports": reports,
            "csv": csv_payload,
        }, 200
    except FileNotFoundError as not_found:
        return {"error": str(not_found)}, 500
    except RuntimeError as runtime_error:
        return {"error": str(runtime_error)}, 500
    except Exception as unexpected:
        return {"error": f"Unexpected failure: {unexpected}"}, 500
    finally:
        for path in (input_path, output_path):
            try:
                if path.exists():
                    path.unlink()
            except OSError:
                pass


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=DEFAULT_PORT, debug=os.environ.get("FLASK_DEBUG") == "1")
