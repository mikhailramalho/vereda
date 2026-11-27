// g is also used in inc, and h's address goes to set, so neither becomes a
// local of main; either one promoted would fail the assertion for every x.
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern unsigned __VERIFIER_nondet_uint(void);
unsigned g, h;
void inc(void) { g++; }
void set(unsigned *p) { *p = 2; }
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  g = x;
  inc();
  set(&h);
  __VERIFIER_assert(g == x + 1 && h == 2);
  return 0;
}
