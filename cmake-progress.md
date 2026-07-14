# CMake Learning Roadmap — Progress Log

Companion file to `cmake-learning-roadmap.md`. Update this after each milestone so any new chat in this project can pick up context without re-explaining.

---

## Milestone 1 — Hello, Build System
**Status:** ✅ Done

- Wrote a `CMakeLists.txt` with `cmake_minimum_required`, `project()`, and `add_executable()`, pulling source from a `src/` subdirectory.
- Built out-of-source in a `build/` directory using the standard configure → build two-step.
- Checkpoint passed: deleted `build/` entirely, rebuilt from scratch using only `cmake ..` and `cmake --build .`.
- Correctly explained that CMake is a generator, not a build tool — `Makefile` is the actual recipe consumed by GNU Make, and CMake never invokes the compiler directly during configure.
- Tried `-G Ninja` out of curiosity; hit the expected error since Ninja isn't installed (`CMake was unable to find a build program corresponding to "Ninja"`) — correctly interpreted as an environment issue, not a CMake bug.

**Notes for next time:** Solid grasp of configure-vs-build distinction. No open questions carried forward.

---

## Milestone 2 — Splitting Into a Library
**Status:** ✅ Done

- Split logic into `print.cpp`/`print.h`, compiled as a static library target `print_hello` via `add_library(print_hello STATIC print.cpp)`.
- Linked `hello_cmake` executable against it with `target_link_libraries(hello_cmake print_hello)`.
- Checkpoint passed: verbose build showed `print.cpp` and `main.cpp` compiled as two independent CXX compile steps into separate `.o` files, followed by a distinct archiving step (`ar qc libprint_hello.a` + `ranlib`) producing the static library, then a final separate link step (`c++ main.cpp.o -o hello_cmake libprint_hello.a`) producing the executable.
- Correctly identified `libprint_hello.a` as the static library artifact and understood the ar/ranlib step as distinct from both compilation and final linking.

**Notes for next time:** Good grasp of the compile → archive → link pipeline for static libraries. No open questions carried forward.

---

## Milestone 3 — Real Directory Structure
**Status:** ✅ Done

- Reorganized into `src/` (`main.cpp`, `print.cpp`) and `include/` (`print.h`), with a top-level `CMakeLists.txt` that just does `add_subdirectory(src)`, and a `src/CMakeLists.txt` defining the actual targets.
- Used `target_include_directories(print_hello PUBLIC ${CMAKE_SOURCE_DIR}/include)` so `hello_cmake` can `#include "print.h"` without a relative path hack.
- Empirically tested all three visibility keywords by breaking things on purpose (not just taking them on faith):
  - **PRIVATE** → `print_hello` compiles fine, `hello_cmake` fails (can't find `print.h`) — include path stops at the library's own boundary, not exported to consumers.
  - **INTERFACE** → `print_hello` itself fails to compile (`print.cpp` can't find `print.h`) — include path only exported outward, not applied to the target's own compile step.
  - **PUBLIC** → both succeed — union of the two behaviors. Correctly identified as the right choice here since `print.h` is needed both to compile the library and by anything linking against it.
- Checkpoint: added a third target, `standalone` (no dependency on `print_hello`), confirmed it builds fine with zero include-path leakage. Went a step further and confirmed the reverse — added a `print_hello` call to `standalone.cpp` without linking the library, got the expected "can't find print.h" error — proving include propagation is tied to the link graph, not just something globally available in the directory.
- Noted but deliberately deferred: executables land in `build/src/` rather than `build/` (mirrors source tree structure by default; fixable later with `RUNTIME_OUTPUT_DIRECTORY`, not a checkpoint requirement).
- Deleted `standalone` target/file after it served its purpose as a scoping test.

**Notes for next time:** Strong grasp of target-scoped includes and *why* they matter over the old directory-level `include_directories()` — tested to failure rather than memorized. No open questions carried forward.

---

## Milestone 4 — Build Types, Flags, and Properties
**Status:** ⬜ Not started

---

## Milestone 5 — Options and a Generated Config Header
**Status:** ⬜ Not started

---

## Milestone 6 — External Dependencies with FetchContent
**Status:** ⬜ Not started

---

## Milestone 7 — Testing with CTest
**Status:** ⬜ Not started

---

## Milestone 8 — Install Rules and Basic Packaging (capstone)
**Status:** ⬜ Not started
