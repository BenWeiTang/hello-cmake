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

**Status:** ✅ Done

- Used `target_compile_features(... cxx_std_17)` on both targets instead of the global `CMAKE_CXX_STANDARD` variable. Correctly scoped: `PRIVATE` on `hello_cmake` (leaf executable, nothing consumes it, no reason to propagate outward) and `PUBLIC` on `print_hello` (needed both to compile the library itself and exported to `hello_cmake`, which links against it).
- Explored an `INTERFACE` library (`project_options`) as a way to deduplicate the repeated `target_compile_features` calls across targets. Understood why `INTERFACE` is the only sensible keyword for a no-source-files target (nothing to apply the property to "privately," only something to export). Deliberately reverted to two explicit calls, correctly judging that with only two targets and one shared property, the pattern's overhead (an extra named target to track) outweighs the deduplication benefit. Noted as a real, useful pattern to reach for once target/property count grows.
- Added `DEBUG_VERBOSE` via `target_compile_definitions()` on both targets (not `add_compile_definitions()`, which is directory-scoped and would leak to targets that don't want it), gated with the nested generator expression `$<$<CONFIG:Debug>:DEBUG_VERBOSE>` rather than defining it unconditionally.
- Built from two separate build directories (`build_d` with `-DCMAKE_BUILD_TYPE=Debug`, `build_r` with `-DCMAKE_BUILD_TYPE=Release`) since the generator (Unix Makefiles) is single-config. Ran both binaries and explicitly confirmed the diagnostic output differs, not just that both binaries run.
- Correctly identified that multi-config generators (Ninja Multi-Config, Visual Studio) don't take `-DCMAKE_BUILD_TYPE` at configure time — the config is chosen at build time instead, e.g. via `--config Release`.
- Initial misstep, corrected: configured a third build directory with `CMAKE_BUILD_TYPE` unset entirely and initially called the result "a release build" because `DEBUG_VERBOSE` was absent. Corrected to understand this is a distinct, unnamed state — no optimization flags at all, not equivalent to Release — since `$<CONFIG:Debug>` evaluating false only means the string didn't match "Debug," not that any Release-like properties were applied. Left as a self-check (compare verbose compiler invocations between `build_r` and the unset build for `-O2`/`-O3`) rather than a blocking requirement.

**Notes for next time:** Solid grasp of target-scoped vs directory-scoped properties and generator expressions. The unset-`CMAKE_BUILD_TYPE` distinction needed a correction — worth a quick sanity check early in Milestone 5 if generated-header behavior ever seems to depend on build type, to make sure the "no build type set" trap doesn't resurface there.

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
