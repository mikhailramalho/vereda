// main.i
typedef struct {
} a;
int b;
void func1() { a c = *(a *)&b; }
// Imported from ESBMC regression/esbmc/github_669-empty-struct
// RUN: --entry func1
// EXPECT: ^VERIFICATION SUCCESSFUL$
