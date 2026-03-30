# FlowerExchange

FlowerExchange is an order-matching project that reads flower orders from CSV, validates them, routes them into per-instrument order books, matches them using price-time priority, and writes execution reports back to CSV.

The project is organized as a small exchange system with:

- trader-side orchestration
- exchange-side validation and routing
- in-memory order books
- a matching engine with a Strategy-based matching policy
- unit, scenario, and large-dataset tests

## Current behavior

- Orders are read from CSV input files.
- Orders are validated for instrument, side, quantity, and price.
- Matching uses price-time priority.
- Trades execute at the resting order price.
- The default application path currently uses per-instrument parallel processing.
- Execution reports are written to CSV output files.

## Supported instruments and validation rules

Supported instruments:

- Rose
- Lavender
- Lotus
- Tulip
- Orchid

Validation rules:

- quantity must be between `10` and `1000`
- quantity must be in steps of `10`
- price must be greater than `0`
- side must map to buy or sell

Accepted side tokens in the CSV parser include:

- `1`, `Buy`, `buy`
- `2`, `Sell`, `sell`

## Tech stack

- C++17
- CMake 3.16+
- Standard library concurrency primitives (`std::async`, `std::mutex`, `std::shared_mutex`, `std::atomic`)
- Tested with Visual Studio/MSBuild generated builds on Windows

## Project structure

```text
flower-exchange/
|-- include/
|   |-- common/
|   |-- exchange/
|   |-- io/
|   |-- model/
|   `-- trader/
|-- src/
|   |-- common/
|   |-- exchange/
|   |-- io/
|   |-- model/
|   |-- tools/
|   `-- trader/
|-- tests/
|   `-- include/test_utils/
|-- data/
|-- generated_data/
|-- output/
|-- build/
|-- CMakeLists.txt
`-- README.md
```

Folder summary:

- `src/`: implementation files for the application, exchange, I/O, model, and tools
- `include/`: public headers for the project modules
- `tests/`: executable-style test programs and scenario helpers
- `data/`: small sample input files used for normal runs and scenario checks
- `generated_data/`: large generated CSV files for performance and stress tests
- `output/`: generated execution-report files
- `build/`: generated build files and compiled binaries

## Main executables

- `flower_exchange`: main application
- `large_dataset_generator`: generates large CSV datasets into `generated_data/`

## Prerequisites

You need:

- a C++17-capable compiler
- CMake 3.16 or newer
- on Windows, Visual Studio Build Tools or Visual Studio with C++ support installed

## Quick start

### 1. Configure the project

Visual Studio 2019 generator:

```powershell
cmake -S . -B build -G "Visual Studio 16 2019" -A x64
```

Visual Studio 2022 generator:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

If you already have a preferred generator/toolchain, use that instead as long as it supports C++17.

### 2. Build the project

Debug build:

```powershell
cmake --build build --config Debug
```

Release build:

```powershell
cmake --build build --config Release
```

### 3. Run the application

Run with default paths:

```powershell
.\build\Debug\flower_exchange.exe
```

Current defaults:

- input: `data/orders.csv`
- output: `output/execution_rep.csv`

Run with custom input and output:

```powershell
.\build\Debug\flower_exchange.exe data/sampleOrder1.csv output/execReports1.csv
```

## Test commands

Run the whole test suite through CTest:

```powershell
ctest --test-dir build -C Debug --output-on-failure
```

Run tests in parallel:

```powershell
ctest --test-dir build -C Debug -j 4 --output-on-failure
```

If you want to run a single test executable directly:

```powershell
.\build\Debug\matching_tests.exe
.\build\Debug\sample_file_tests.exe
.\build\Debug\large_dataset_tests.exe
```

## Large dataset workflow

Generate the large test datasets:

```powershell
.\build\Debug\large_dataset_generator.exe
```

This writes datasets such as:

- `generated_data/large_balanced_10k.csv`
- `generated_data/large_balanced_100k.csv`
- `generated_data/large_matching_100k.csv`
- `generated_data/large_non_crossing_100k.csv`
- `generated_data/large_partial_fill_100k.csv`
- `generated_data/large_invalid_mix_100k.csv`
- `generated_data/large_hotspot_100k.csv`
- `generated_data/large_balanced_1m.csv`

Run the large dataset test after generation:

```powershell
.\build\Debug\large_dataset_tests.exe
```

When using `ctest`, dataset generation is already wired in as a fixture step before `LargeDatasetTests`.

## Input and output format

### Input CSV

The application skips the first row as a header and parses each remaining non-empty row into an order.

Typical columns:

```text
client_order_id,instrument,side,quantity,price
```

Example:

```text
client-100,Rose,1,100,55.00
```

### Output CSV

Execution report columns:

```text
order_id,client_order_id,instrument,side,status,executed_quantity,executed_price,reason,transaction_time
```

## Architecture overview

The project uses a practical layered structure:

- `TraderApplication` coordinates end-to-end processing from input file to output file
- `OrderSender` is the trader-side submission seam into the exchange layer
- `ExchangeApplication` assigns exchange metadata, validates orders, and delegates processing
- `Exchange` routes validated orders to the correct instrument book
- `OrderBook` owns one buy side and one sell side per instrument
- `OrderBookSide` maintains price-time priority on one side of the book
- `MatchingEngine` runs the match loop
- `IMatchingStrategy` defines the match policy seam
- `PriceTimePriorityStrategy` preserves the current matching policy
- `CsvOrderReader` and `CsvExecutionReportWriter` handle file I/O

## Matching policy

The current strategy implementation is:

- `PriceTimePriorityStrategy`

That means:

- better price gets priority
- equal price uses earlier sequence number
- execution price is the resting order's price

## Concurrency note

The current application path uses per-instrument parallelism:

- orders are grouped by instrument
- each instrument bucket is processed on its own async task
- orders remain sequential within each instrument so price-time priority is preserved

This keeps matching behavior intact while allowing different instruments to process concurrently.

## Useful commands

Clean rebuild from scratch:

```powershell
Remove-Item -Recurse -Force build
cmake -S . -B build -G "Visual Studio 16 2019" -A x64
cmake --build build --config Debug
```

Run the app with a sample file:

```powershell
.\build\Debug\flower_exchange.exe data/sampleOrder2.csv output/execReports2.csv
```

Run one scenario-oriented test:

```powershell
.\build\Debug\scenario_example4_tests.exe
```

## Frontend viewer

The `web/` folder contains a lightweight execution-report viewer that talks to a Flask bridge in `server/`. To launch it locally:

1. Build `flower_exchange` first so the Flask app can find the binary (or set `FLOWER_EXCHANGE_BIN` to the compiled executable).
2. Create/activate a Python environment and install the single dependency:

   ```powershell
   python -m venv .venv
   . .\.venv\Scripts\Activate.ps1
   pip install -r server/requirements.txt
   ```

3. Start the Flask server:

   ```powershell
   python server/app.py
   ```

4. Visit http://localhost:8080 to load the viewer. Use the “Load Sample Data” button or upload a CSV; the server pipes the file into `flower_exchange` and streams the execution reports back to the UI. Set `FLOWER_EXCHANGE_BIN` if your binary lives outside `build/`.

## Notes

- `README.md` documents the current codebase behavior and structure.
- `generated_data/` can be regenerated at any time using `large_dataset_generator`.
- `output/` is intended for generated execution reports.
- The codebase is built around readability and testability rather than external dependencies.
