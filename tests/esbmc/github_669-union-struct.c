// main.i
typedef union { int x;
} a;
struct { int x; } b;
void func1() { a c = *(a *)&b; }
// Imported from ESBMC regression/esbmc/github_669-union-struct
// RUN: --entry func1
// EXPECT: ^\s*dereference failure:
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s*dereference failure:
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
