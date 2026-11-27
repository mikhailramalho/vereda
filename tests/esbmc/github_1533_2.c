// Minimal reproducer: VLA with zero size from global variable.
// 'a' is a global int, initialized to 0.
// 'int b[a]' is a zero-length VLA, which is undefined behaviour (C11 §6.7.6.2p1).
int a;
int main(void) { int b[a]; return 0; }
// Imported from ESBMC regression/esbmc/github_1533_2
// RUN: --unwind 1 --no-unwinding-assertions --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --solver z3
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: VLA array dimension must be greater than zero
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
