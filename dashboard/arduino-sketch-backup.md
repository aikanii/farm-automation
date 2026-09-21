# Original Arduino Sketch

The original sketch file is preserved at:
[../group 5 final project sketch.txt](../group%205%20final%20project%20sketch.txt)

This file contains the original code before the v2.0.0 refactor.

## Key Differences from v2.0.0

| Aspect | v1.0.0 (Original) | v2.0.0 (Refactored) |
|--------|-------------------|---------------------|
| Configuration | Hardcoded in sketch | Separate `config.h` file |
| Timing | `delay()` blocking | `millis()` non-blocking |
| Error handling | None | Retry logic with fallback |
| Variables | `x`, `manual` | `manualBulbState`, `controlMode` |
| Code structure | Single file, minimal comments | Well-organized with sections |
| Sensor reads | Single attempt | 3 retries with averaging |
| LCD display | Basic text | Custom icons + mode display |
| Documentation | None | Comprehensive README + guides |
