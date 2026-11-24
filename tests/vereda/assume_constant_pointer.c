// EXPECT: ^VERIFICATION SUCCESSFUL$
// Pointers are equal when object and offset are, so after p == &b[0] the
// path has p naming b, not the one-past-the-end pointer of a (#120).
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int a[1], b[1];
int main(void) {
  int *p = __VERIFIER_nondet_int() ? &a[1] : &b[0];
  __VERIFIER_assume(p == &b[0]);
  *p = 1;
  __VERIFIER_assert(b[0] == 1);
  return 0;
}
