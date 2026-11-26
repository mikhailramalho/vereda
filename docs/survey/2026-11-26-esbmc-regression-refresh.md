# ESBMC regression refresh

Import new C fixtures from the local ESBMC checkout at
`bb8142f6f6` using `tests/import_esbmc.py`. Import into a staging directory
and compare the `Imported from ESBMC` origins against the tracked Vereda
fixtures before copying additions. Existing fixtures and their classifications
stay unchanged. Only tracked ESBMC files are copied, excluding generated
witnesses and other local artifacts.

The refresh adds 291 tests: 72 CORE and 219 FUTURE. Run the importer marking
logic against the verifier built for the struct-field partition change on
main, using LLVM 23.1.6, a 300-second timeout and a 4096 MB memory limit per
test, with three concurrent workers. Record the observed verdicts for FUTURE
tests; unsupported flags and intrinsics retain their Needs annotations.
After marking, a second run of all 291 additions gives 72 passes and 219
expected skips, with no failures. The importer regression checks also pass.

The new `esbmc-fixedbv` suite contains 27 tests covering fixed-point arithmetic,
conversion, saturation, shifts, bitcasts and builtin handling. Its ordinary
C `user_defined_sqrt` fixture passes. The other 26 are FUTURE because the C
front end currently does not enable Clang's `-ffixed-point` option; importing
them does not add fixed-point verification support. Two additional upstream
fixed-point fixtures use ESBMC function contracts and remain excluded under
the existing importer policy.

Upstream moved the experimental bitwuzllob and neurosym wrapper fixtures to
`--smtlib-oneshot-prog` and `--smtlib-oneshot-model-prog`. Extend the existing
wrapper exclusion to these names rather than reintroducing 32 previously
excluded fixtures. Cover both spellings in the importer regression checks.
