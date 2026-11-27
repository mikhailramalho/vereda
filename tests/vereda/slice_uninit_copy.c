// Under the uninit check the slice keeps every store: copying a, whose
// padding is uninitialised, into g, which nothing reads, is no read of the
// padding, and without the store the copy's load would be checked on its
// own.
// RUN: --ub-uninit-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
struct s { char c; int i; };
struct s g;
int main(void) {
  struct s a;
  a.c = 1;
  a.i = 2;
  g = a;
  return 0;
}
