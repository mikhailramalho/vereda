// An --unwindsetname entry without its bound is an error.
// RUN: --unwindsetname main:0
// EXPECT: --unwindsetname: bad entry main:0$
int main(void) { return 0; }
