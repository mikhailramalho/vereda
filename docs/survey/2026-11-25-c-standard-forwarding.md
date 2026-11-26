# C language standard forwarding

The next priority after recovering imported coverage in #255 is support for
real source code. ESBMC/CPROVER-specific intrinsics remain lower priority than
language and library features useful outside verification benchmarks.

`vereda.py --std c11 file.c` now selects clang's C dialect. Both `--std=c11`
and clang's `-std=c11` spelling work, and repeated options select the last
value. The flag reaches every C compilation, including additional translation
units and preprocessed `.i` inputs, without reaching llvm-link or the verifier.
Clang validates the standard name so the frontend need not duplicate its list.
Without the option, the existing compiler default remains in use. Rust and
IR-only inputs reject the option because no C compilation would consume it.

The importer now preserves `--std` as a supported valued option. Refreshing the
84 imported headers that listed it as missing changes only test metadata;
source bodies and expectations are unchanged. Eight tests can be promoted to
CORE: the passing/failing C23 boolean literals, `github_10786_side_effect`,
`github_7466`, and the passing/failing pairs for `github_9228_alignment` and
`cstd/github_9352`.

Twelve other tests have no remaining declared option blocker but still fail
an expectation. Six need external Ubuntu kernel headers. Four `github_10383`
tests use undeclared verification functions, which C23 rejects. `github_10786`
expects division-by-zero checks that are not emitted, and `github_7466_fail`
expects ESBMC's assertion wording. Their detailed expectations remain intact
and they stay FUTURE. An additional experiment with C23 empty VLA initialization
compiled successfully but reached the existing unsupported symbolic-size memset;
standard selection alone does not supply every runtime operation a dialect can
produce. No symbolic memset or verification intrinsic support was added here.

Regression coverage checks actual standard-version and strict-mode macros for
C11, GNU17, C23 and GNU89, and C23 keywords in preprocessed input. Frontend tests
exercise multiple translation units, mixed C/IR input, invalid and empty names,
a missing value, repeated options, and Rust/IR-only rejection. Importer tests
cover both valued-option spellings and the absence of a missing-feature marker.

The Release build with LLVM 23.1.6 passes the complete CTest run: 7,031
executions, 2,918 passes, 4,113 expected skips, and zero failures in 300.78
seconds with eight workers. Compared with main after #255, this adds six new
executions and recovers eight existing fixtures, reducing skips from 4,121.

Follow-up issues: #292.
