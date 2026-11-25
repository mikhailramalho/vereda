// As summary_flush_whole, with a[3] left out of the expected values.
// EXPECT: ^\[INFO\] Summarised 1 array loop\(s\)$
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int a[100];
  for (int i = 0; i < 100; i++)
    a[i] = i;
  int n = __VERIFIER_nondet_int();
  if (n < 0 || n >= 100)
    return 0;
  a[n] = 5;
  a[3] = 7;
  int m = __VERIFIER_nondet_int();
  if (m < 0 || m >= 100)
    return 0;
  __VERIFIER_assert(a[m] == (m == n ? 5 : m));
  return 0;
}
