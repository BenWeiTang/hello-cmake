# CMake Learning Roadmap: Absolute Basics → Medium Techniques

## How this works

Each milestone has:
- **Concepts** — the small set of vocabulary/commands you need (not a full tutorial)
- **Project** — a concrete thing you'll build with CMake
- **Your task** — what to try, without a handed-to-you solution
- **Checkpoint** — a way to verify you actually got it right, before moving on
- **Common traps** — mistakes people hit at this stage, named but not explained away

Don't skip the checkpoints. If a checkpoint fails, that's the useful part — debug it before asking for the answer.

---

## Milestone 1 — Hello, Build System

**Concepts:** `cmake_minimum_required()`, `project()`, `add_executable()`, out-of-source builds, the two-step build (`cmake` generates, then `cmake --build` compiles).

**Project:** A single `main.cpp` that prints something, built entirely through CMake (no calling `g++` directly).

**Your task:**
- Write a `CMakeLists.txt` with the three commands above.
- Configure into a separate `build/` directory (don't build in the source folder — figure out why that's the convention, don't just take it on faith).
- Build and run the binary.

**Checkpoint:** Delete your `build/` folder entirely and rebuild from scratch with two commands. If you need more than two, something's off. Also: explain to yourself (or to me) what `cmake ..` actually *produces* before `cmake --build .` is run — if you can't say what's in that build folder, dig in with `ls`.

**Common traps:** forgetting `cmake_minimum_required` (some generators behave oddly without it), building inside the source tree and then wondering why `.gitignore` matters, confusing the *configure* step with the *build* step.

---

## Milestone 2 — Splitting Into a Library

**Concepts:** `add_library()`, `target_link_libraries()`, the idea of a "target" as CMake's core unit (not a file, not a folder — a target).

**Project:** Take a small piece of logic (e.g. a couple of math or string-utility functions) out of `main.cpp` into its own `.cpp`/`.h` pair, compiled as a library, and linked into your executable.

**Your task:**
- Create a static library target from the new source file.
- Link your executable against it.
- Don't use `add_subdirectory` yet — keep everything in one `CMakeLists.txt` for now, just multiple targets.

**Checkpoint:** Run your build with verbose output (look up the flag for your generator) and find the actual compiler invocation for the library `.cpp` file. Confirm it's compiled *separately* from `main.cpp` and then linked, not compiled together. If you can't find two distinct compile steps, you haven't actually split anything yet.

**Common traps:** forgetting to link the library (compiles fine, fails at link time — learn to read that error), putting the header include path in the wrong place instead of using `target_include_directories`.

---

## Milestone 3 — Real Directory Structure

**Concepts:** `add_subdirectory()`, `target_include_directories()` with `PUBLIC`/`PRIVATE`/`INTERFACE`, project layout conventions (`src/`, `include/`).

**Project:** Reorganize Milestone 2 into:
```
project/
  CMakeLists.txt        (top-level)
  src/
    CMakeLists.txt
    main.cpp
    mathlib.cpp
  include/
    mathlib.h
```

**Your task:**
- Move the library into `src/` with its own `CMakeLists.txt`, pulled in via `add_subdirectory`.
- Use `target_include_directories` so the executable can find `mathlib.h` without a relative `../include` path hack.
- Figure out, by testing, the actual difference between `PUBLIC`, `PRIVATE`, and `INTERFACE` for include directories — don't just copy whichever one compiles.

**Checkpoint:** Add a second `.cpp` file to the executable that does *not* need the library, and confirm it still builds fine — meaning your include propagation is scoped correctly, not just globally hacked with `include_directories()` at the top level (avoid that command entirely at this stage).

**Common traps:** using the directory-level `include_directories()` instead of target-scoped includes — it "works" but teaches the wrong habit; getting `PUBLIC` vs `PRIVATE` backwards and not noticing until a third target needs the headers.

---

## Milestone 4 — Build Types, Flags, and Properties

**Concepts:** `CMAKE_BUILD_TYPE` (Debug/Release/RelWithDebInfo), `target_compile_options()`, `target_compile_definitions()`, `target_compile_features()` (for C++ standard).

**Project:** Same project, but make it standard-aware and configuration-aware: require C++17 (or whichever you're using), and make a debug build print extra diagnostic output that a release build doesn't.

**Your task:**
- Set the C++ standard via `target_compile_features` (not the old global `CMAKE_CXX_STANDARD` variable — try the modern target-based way first).
- Add a compile definition like `DEBUG_VERBOSE` that's only defined in Debug builds, and gate some `#ifdef` output in your code on it.
- Build both `Debug` and `Release` configurations side by side (single-config generators need two separate build directories — figure out why).

**Checkpoint:** Produce a Debug build and a Release build from the *same source tree* without cross-contaminating each other, and show the verbose output differs between them. If your Release build still prints debug output, your compile definition isn't scoped to the build type correctly.

**Common traps:** assuming `CMAKE_BUILD_TYPE` works the same on all generators (it doesn't, for multi-config ones like Visual Studio/Ninja Multi-Config), forgetting that an unset `CMAKE_BUILD_TYPE` on single-config generators means *no* optimization flags at all.

---

## Milestone 5 — Options and a Generated Config Header

**Concepts:** `option()`, `configure_file()`, generating a `config.h.in` → `config.h`.

**Project:** Add a user-toggleable feature to your app (e.g. `ENABLE_LOGGING`) controlled at configure time, with the value baked into a generated header your code `#include`s.

**Your task:**
- Declare an `option()` with a sensible default.
- Write a `config.h.in` template with a `#cmakedefine` or `@VAR@` placeholder.
- Use `configure_file()` to produce the real header into the build directory, and make sure your target can actually find it (this is where include-path scoping from Milestone 3 gets tested again).
- Wire the feature toggle into actual behavior in `main.cpp`.

**Checkpoint:** Reconfigure with the option flipped (`-DENABLE_LOGGING=OFF` vs `ON`) and confirm the generated header actually changes without you touching it by hand. Then delete the generated header and confirm CMake regenerates it on the next build — if it doesn't, you've hardcoded something that should be templated.

**Common traps:** editing the generated header directly instead of the `.in` template (it gets clobbered on next configure — that's the lesson), forgetting the generated header lives in the *build* tree, not the source tree, and needing that path added to includes.

---

## Milestone 6 — External Dependencies with FetchContent

**Concepts:** `FetchContent_Declare()`, `FetchContent_MakeAvailable()`, why this differs from manually vendoring a library or using `find_package()`.

**Project:** Pull in a small header-only library (e.g. `{fmt}` or `nlohmann/json`) and use it for real in your app — e.g. replace your manual print statements with `fmt::print`, or emit a small JSON config.

**Your task:**
- Add a `FetchContent` block declaring the dependency by Git repo and tag/version (pin a version — don't float on a branch).
- Link your executable against the fetched target.
- Get it building with zero manually-downloaded files — everything pulled at configure time.

**Checkpoint:** Clean your entire build directory *and* whatever local cache FetchContent uses, then rebuild from nothing and confirm it fetches and builds correctly, offline-failure aside. Then explain to yourself why pinning a specific tag/commit matters here instead of tracking `main`.

**Common traps:** not pinning a version (reproducibility breaks silently later), linking against the wrong target name (library authors don't always name it what you'd guess — you have to check).

---

## Milestone 7 — Testing with CTest

**Concepts:** `enable_testing()`, `add_test()`, integrating a test framework (Catch2 or GoogleTest, pulled in via the FetchContent skill from Milestone 6), running tests via `ctest`.

**Project:** Add real unit tests for your math/utility library from Milestone 2, using a proper test framework, runnable via `ctest` from the build directory.

**Your task:**
- Fetch a test framework as a dependency.
- Write a handful of actual test cases against your library code (not the executable — the library, directly).
- Register the test binary with `add_test()`.
- Get `ctest` to report pass/fail without you manually running the test executable.

**Checkpoint:** Intentionally break one function in your library, rerun `ctest`, and confirm it reports a failure with a nonzero exit code (check `$?` after running it, or how your CI would interpret it). Then fix it and confirm green again. If breaking the code doesn't produce a visible failure, your test isn't actually wired to `add_test()` correctly.

**Common traps:** writing tests that always pass because they're not asserting anything meaningful, forgetting `enable_testing()` at the top level (subdirectory tests silently don't register).

---

## Milestone 8 — Install Rules and Basic Packaging (capstone)

**Concepts:** `install(TARGETS ...)`, `install(FILES ...)`, `CMAKE_INSTALL_PREFIX`, and a light touch of `CPack` if you want to go further.

**Project:** Make your whole project (library + executable + headers) installable to a staging directory, pulling together everything from Milestones 1–7 into one coherent, tested, installable package.

**Your task:**
- Add `install()` rules for your executable, your library, and your public headers.
- Install to a local staging prefix (not system-wide) and inspect the resulting layout — does it look like a normal Unix install tree (`bin/`, `lib/`, `include/`)?
- Optionally: add a minimal `CPack` config and generate a `.tar.gz` or `.zip` package.

**Checkpoint:** From a completely clean checkout, run configure → build → test → install in sequence, and confirm a second, independent project could `find_package()` or manually link against your installed library using only what landed in the install prefix — not your build tree.

**Common traps:** forgetting to install headers (library installs fine, but nothing can `#include` it afterward), hardcoding absolute paths instead of using `CMAKE_INSTALL_PREFIX`-relative ones.

---

## After this

At this point you have: targets, scoped includes, build types, generated config, external deps, tests, and install rules — which is genuinely "medium level" CMake and covers what most real-world C++ projects actually use day to day. Natural next steps beyond this roadmap (not covered here) would be: writing your own `Find<Package>.cmake` modules, cross-compiling / toolchain files, and generator expressions (`$<...>`) for more advanced conditional builds.

## How to use this with me

Come back milestone by milestone. Tell me what you tried and what happened (including errors, verbatim) — I'll help you debug and point you toward the right command or concept rather than handing over a finished `CMakeLists.txt`, unless you've been stuck a while and want to see a full solution to compare against.
