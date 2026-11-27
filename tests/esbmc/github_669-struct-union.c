// main.i
typedef struct { int x;
} a;
union { int x; } b;
void func1() { a c = *(a *)&b; }
// Imported from ESBMC regression/esbmc/github_669-struct-union
// RUN: --entry func1 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
