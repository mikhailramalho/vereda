# --no-standard-checks in the importer

Recorded 2026-11-28.

ESBMC's --no-standard-checks turns off its pointer, division-by-zero,
pointer-relation, scanf, VLA-size, alignment and bounds checks. Vereda
has flags for three of them, so the importer now translates it, and
ESBMC's spelling --no-standard-check, into --ub-pointer-check=false
--ub-div-by-zero-check=false --vla-size-check=false. The other four
are checks Vereda does not have, which is why the importer already
drops --no-bounds-check, --no-align-check and
--no-pointer-relation-check.

The flag was not added to the driver. The driver takes none of ESBMC's
--no-*-check spellings: the importer translates each of them into
Vereda's --ub-*-check=false (RENAMED in tests/import_esbmc.py), and
2026-11-26-imported-option-cleanup.md kept ESBMC's modes out of the
binary. --no-standard-checks is one more of those spellings.

43 imported tests needed no other flag. Their headers were rewritten as
the importer now writes them, and the importer's mark() ran them: 33
pass and are CORE. 10 stay FUTURE with their verdicts recorded: 7 are
unsupported (pthread_create in the two github_9441_atomic_narrow_rmw
tests, a computed goto in the five github_4083 tests), the two
github_7597-chain-dropped tests expect a warning only ESBMC prints, and
github_9427 fails because Vereda has no model of atan.
