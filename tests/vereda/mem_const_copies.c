// Constant globals the program did not declare: clang's copies of the
// initialisers of table and buf, copied into the locals, which the program
// then writes. Neither those writes nor the one into g, which is not
// const, is a violation.
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: dereference failure
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int g = 5;

int main(void) {
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 3);
  int table[3] = {1, 2, 3};
  char buf[] = "abc";
  buf[i] = 'x';
  int local = 0;
  int *p = __VERIFIER_nondet_int() ? &g : &local;
  *p = table[i];
  __VERIFIER_assert(buf[i] == 'x' && *p == table[i] && table[i] > 0);
  return 0;
}
