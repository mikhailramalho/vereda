// A loop whose values start where a summarised loop before it left them:
// SCEV names each start by an AddRec of the earlier loop, whose value is the
// term of the LCSSA phi that carries it out.
// EXPECT: ^Summarised 2 array loop\(s\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
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
  __VERIFIER_assert(x == n);
  return 0;
}
