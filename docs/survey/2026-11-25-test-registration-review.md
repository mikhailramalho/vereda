# Registration fixture compatibility and benchmark priority

Review of PR #252 identified an interaction with PR #249: the registration
test copies `tests/CMakeLists.txt` into a `project(RunnerTests NONE)` fixture.
Unconditional C++ unit-test targets would make that fixture fail without a
C++ compiler, the target's source, or libvereda. Guard the unit-test section
with `if(TARGET libvereda)`; the source-regression registration loop remains
available without the library. Production builds define libvereda and keep
their unit tests.

The generated fixture now injects PR #249's table-pattern target declaration
before configuring, deliberately omitting its source and library. It still
checks C and IR configurations, separate run indices, and automatic
reconfiguration when RUN directives change.

The initial candidate ordering in `2026-11-25-test-configurations.md` put
head-only cases first. Replace that rule with potential regressions first,
then head-only cases, then potential improvements, retaining the 40-case
cap and relative-change ranking within the measured categories. A bulk
import therefore cannot crowd an existing regression out with new fixtures.
A test supplies more than 40 new cases alongside a regression and an
improvement, and verifies that the regression is selected first.

All 14 infrastructure tests pass. The production CMake configuration still
registers the verifier's unit tests. The local rebuild took 1.2 seconds to
configure and 0.4 seconds to generate across the suite; retain the standard
CMAKE_CONFIGURE_DEPENDS tracking so RUN edits continue to update test
registration automatically.
