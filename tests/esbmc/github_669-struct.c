// main.i
typedef struct { float x;
} a;
int b;
void func1() { a c = *(a *)&b; }
// Imported from ESBMC regression/esbmc/github_669-struct
// RUN: --entry func1
// EXPECT: ^VERIFICATION SUCCESSFUL$
