# Flower Exchange

A matching engine that ingests CSV orders, validates them, routes them through the exchange, and emits execution reports. The repo now includes a lightweight Flask server plus a neon-inspired frontend so you can upload custom datasets, inspect execution reports, and download the generated CSV.

## Build the matching engine

```bash
cmake -S . -B build
cmake --build build
```

The binary accepts optional CLI arguments:

```bash
# Default sample input/output
./build/flower_exchange

# Custom run
./build/flower_exchange input/orders.csv output/execution_rep.csv
```

## Launch the web experience

1. Ensure the binary above is built (or set `FLOWER_EXCHANGE_BIN` to its path).
2. Install the small Python dependency set:
   ```bash
   python -m venv .venv
   .venv\Scripts\activate  # PowerShell on Windows
   pip install -r server/requirements.txt
   ```
3. Start the Flask server:
   ```bash
   python server/app.py
   ```
4. Open [http://localhost:8080](http://localhost:8080) and use the UI to drag/drop an `orders.csv`, load the curated sample data, filter rows, and download the generated execution report.

> The server looks for the engine in `build/flower_exchange(.exe)` by default. Set `FLOWER_EXCHANGE_BIN` if you keep artifacts elsewhere (for example `build/Debug/flower_exchange.exe`).

## API overview

`POST /api/execute` accepts a multipart form field named `orders`, saves it to a temp location, executes the C++ engine, returns the parsed JSON plus the raw CSV payload. The same Flask app serves everything under `/web`, so the SPA and API share a single origin.

## Repository structure

- `src/` & `include/` – core exchange, reader, writer, and trader orchestration code.
- `tests/` – regression and unit coverage for the engine.
- `web/` – the static frontend (Space Mono + DM Sans palette, upload zone, stats band, interactive table).
- `server/` – Flask glue that shells out to the compiled binary, parses the execution report, and exposes the API.

## Next steps

- Extend the API to stream progress updates during large uploads.
- Containerize the server + engine for easier deployment.
- Add auth/tenant scoping if you plan to expose the UI beyond local use.
