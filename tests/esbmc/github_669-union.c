// main.i
typedef union { float x;
} a;
int b;
void func1() { a c = *(a *)&b; }
// Imported from ESBMC regression/esbmc/github_669-union
// RUN: --entry func1 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
