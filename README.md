# Real Estate Sales Management (RESMS)

## Fork: error handling, logging, and assignment write-up
The changes below satisfy a coursework assignment: **targeted error handling**, **stderr logging**, and **documentation** of logging choices.

### Issues addressed

| Area | Problem |
|------|---------|
| Input | `cin` used without checking stream state; bad input could leave the menu in a bad state. |
| File I/O | Opens on `passwords.in` / `data.in` were not checked for missing files or I/O failures. |
| `eof()` loops | `while (!file.eof())` with `>>` could duplicate the last record and mis-parse tokens. |
| `changepass` | Previous logic could not persist updates (wrong use of `list` / `pop_front`). |
| `load` | List was passed **by value**, so `main` never received loaded rows. |

### Strategy

- Check `cin` / `ifstream` / `ofstream` after critical operations; on failure log **ERROR**, use `clear`/`ignore` where appropriate.
- Verify `is_open()`, `bad()`, `good()`; **INFO** when `data.in` loads (with row count).
- Read passwords with `while (file >> pas >> ag)`; rewrite `changepass` with a vector of pairs and a single write of `passwords.in`.
- Validate `data.in` lines (at least five `:` separators); **WARNING** and skip malformed rows.
- **`SIGINT`:** handler logs and exits with code **130** (POSIX-style), analogous to clean `KeyboardInterrupt` handling.

### Logging

- **Where:** `logging.h` → **stderr**, timestamp + level + logger name `resms` (stdout stays for prompts).
- **INFO:** start/end, successful login, load count, list sort, password file update, menu option, unimplemented stubs.
- **WARNING:** bad sort/menu choice, failed login, malformed rows, empty selling price.
- **ERROR:** cannot open files, stream/read/write failures.

---

## AI vs human-oriented logging

| Often AI-first | Often human-first |
|----------------|-------------------|
| Debug-everything logs that duplicate the UI | Decisions and anomalies: bad input, skipped rows, I/O failure |
| Broad patterns that hide bugs | Explicit stream/file checks; avoid `eof()`-driven token loops |
| Logging full credentials or PII at INFO | Event-level logs; in production, redact or avoid PII |

AI suggestions are a useful checklist; **signal-to-noise**, **privacy**, and **which failures to surface** are human judgment calls.

---

## Project files

| File | Role |
|------|------|
| `project.cpp` | Main CLI: auth, load, menu, `addProperty`, `changepass`, `logging` |
| `logging.h` | `sales_log::info` / `warning` / `error` to stderr |
| `Data.h` | Property record type and comparison operators |
| `Property.h`, `Agent.h` | Object model (reference) |
| `data.in`, `passwords.in` | Sample data |
| `a1login.cpp` | Separate login tutorial sample |
| `lista.h`, `nodo.h`, `makefile` | List template / legacy makefile (not the main RESMS build) |

---

