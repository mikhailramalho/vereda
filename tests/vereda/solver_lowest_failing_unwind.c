// Both loops can be cut short; the first one's unwinding assertion is
// reported, as under --incremental-solver.
// RUN: --unwind 1
// EXPECT: FAILED +\[main\.assertion\.1\] +line 12
// EXPECT-NOT: unwinding assertion loop 1
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int n = __VERIFIER_nondet_int();
  int s = 0;
  if (n > 10)
    while (s < n)
      s += 3;
  if (n != 7)
    while (s != 4)
      s++;
  return s;
}
