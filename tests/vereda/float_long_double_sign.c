// Negation and fabsl on long double (x86_fp80).
// RUN: -m64
// RUN: -m64 --fp2bv
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern long double fabsl(long double);
extern long double x;
long double y;
int main(void) {
  y = -x;
  y = fabsl(y);
  return 0;
}
