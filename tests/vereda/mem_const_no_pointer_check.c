// --ub-pointer-check=false, as ESBMC's --no-pointer-check, leaves writes
// into a string literal or a const object unchecked: the write happens and
// the run goes on.
// RUN: --ub-pointer-check=false
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  __VERIFIER_assert$
// EXPECT-NOT: dereference failure
extern void __VERIFIER_assert(int);

const int limit = 10;

int main(void) {
  char *p = "abc";
  p[0] = 'x';
  *(int *)&limit = 5;
  __VERIFIER_assert(p[0] == 'a');
  return 0;
}
