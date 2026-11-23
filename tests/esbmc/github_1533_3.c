// Regression for negative VLA dimension (complement to github_1533_2).
// 'a' is a global int, initialized to -1.
// 'int b[a]' is a negative-length VLA, which is undefined behaviour (C11 §6.7.6.2p1).
int a = -1;
int main(void) { int b[a]; return 0; }
// Imported from ESBMC regression/esbmc/github_1533_3
// RUN: --unwind 1 --no-unwinding-assertions --overflow-check --solver z3
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: VLA array dimension must be greater than zero
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
