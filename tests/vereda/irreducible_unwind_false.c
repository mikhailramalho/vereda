// irreducible_unwind.c one bound short, as ESBMC 8.3 finds with --unwind 5.
// No loop statement places the loop, so it is at a jump back to the head
// FixIrreducible adds, here a's i++ falling into b (ESBMC: the if, line 18).
// RUN: --unwind 5
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 15  unwinding assertion loop 0$
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int i = 0;
  if (__VERIFIER_nondet_int())
    goto b;
a:
  i++;
b:
  i++;
  if (i < 10)
    goto a;
  __VERIFIER_assert(i == 10 || i == 11);
  return 0;
}
