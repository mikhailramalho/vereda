// Names in a counterexample: a union store is the member of its size, a
// bitfield store (which writes the whole unit) is not shown, a parameter
// assigned from another parameter is, and an uninitialised local copied
// into a global keeps its declaration with its value.
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  u\.i = 300 \(
// EXPECT-NOT: u\.c =|bf\.
// EXPECT: ^  v = 2 \(
// EXPECT: ^  x = \S+ \([01 ]+\)\n\nState \d+ file \S+ line 16 column \d+ function main thread 0\n-+\n  g = 
extern void __VERIFIER_assert(int);
union U { char c; int i; } u;
struct B { unsigned a : 3, b : 5; } bf;
int g;
static int f(int v, int w) { v = w; return v; }
int main(void) {
  int x; g = x;
  u.i = 300;
  bf.b = 3;
  __VERIFIER_assert(f(1, 2) != 2 || g != x);
  return 0;
}
