// A loop made of gotos, entered at a or, by the first goto, at b. Each pass
// through the head FixIrreducible adds is one iteration: the goto path runs
// i from 1 to 11 in 6 passes, and ESBMC 8.3 needs --unwind 6 too.
// RUN: --unwind 6
// EXPECT: ^  SUCCESS      \[main\.assertion\.1\]  line 15  unwinding assertion loop 0$
// EXPECT: ^VERIFICATION SUCCESSFUL$
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
