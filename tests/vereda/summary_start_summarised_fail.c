// A loop whose values start where a summarised loop before it left them
// keeps the counts the claim gets wrong.
// EXPECT: ^\[INFO\] Summarised 2 array loop\(s\)$
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  unsigned x = n, y = 0;
  while (x > 0) {
    x--;
    y++;
  }
  unsigned z = y;
  while (z > 0) {
    x++;
    z--;
  }
  __VERIFIER_assert(x < 1000);
  return 0;
}
