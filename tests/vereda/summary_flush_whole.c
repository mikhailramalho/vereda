// A store at a symbolic index after a summarised loop, then one at a
// constant index, which moves to the array when a[m] is read. The element
// is written whole, so the move reads nothing below it, and the summary is
// constrained only where a read reaches it.
// EXPECT: ^Summarised 1 array loop\(s\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
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
  __VERIFIER_assert(a[m] == (m == 3 ? 7 : m == n ? 5 : m));
  return 0;
}
