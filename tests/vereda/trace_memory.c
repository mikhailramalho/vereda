// A counterexample names what a store into memory writes, as ESBMC does:
// an array element, a member, a two-dimensional element, an element at a
// nondet index (with the index the model chose), and a local whose address
// is taken. A parameter's binding to its argument is no assignment; a later
// one is.
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  g\[1\] = 5 \(
// EXPECT: ^  s\.y = 7 \(
// EXPECT: ^  m\[1\]\[2\] = 9 \(
// EXPECT: ^  g\[3\] = 4 \(
// EXPECT: ^  local = 6 \(
// EXPECT-NOT: local = 0
// EXPECT: ^State \d+ file \S+ line \d+ function bump thread 0\n-+\n  v = 2 \(
// EXPECT-NOT: function bump thread 0\n-+\n  v = 1 \(
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);
int g[4];
struct point { int x, y; } s;
short m[2][3];
static void set(int *p, int v) { *p = v; }
static int bump(int v) {
  v = v + 1;
  return v;
}
int main(void) {
  g[1] = 5;
  s.y = 7;
  m[1][2] = 9;
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i == 3);
  g[i] = 4;
  int local;
  set(&local, 6);
  __VERIFIER_assert(bump(1) != 2 || local != 6);
  return 0;
}
