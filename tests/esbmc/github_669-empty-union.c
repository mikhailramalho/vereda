// main.i
typedef union {
} a;
int b;
void func1() { a c = *(a *)&b; }
// Imported from ESBMC regression/esbmc/github_669-empty-union
// RUN: --entry func1 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
