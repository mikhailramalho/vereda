// Large splat non-zero constant arrays (whole, nested, and inside a struct)
// use a constant array or partial-range lambda instead of element-by-element
// stores.
// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);
static int ones[2048] = { [0 ... 2047] = 1 };
static int mat[64][64] = { [0 ... 63] = { [0 ... 63] = 0x12345678 } };
struct T {
  int head;
  int vals[2048];
  int tail;
};
static struct T g = { 5, { [0 ... 2047] = 0x01020304 }, 9 };
int main(void) {
  unsigned i = __VERIFIER_nondet_uint(), r = __VERIFIER_nondet_uint(),
           c = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 2048 && r < 64 && c < 64);
  __VERIFIER_assert(ones[0] == 1 && ones[i] == 1);
  __VERIFIER_assert(mat[r][c] == 0x12345678);
  __VERIFIER_assert(g.head == 5 && g.tail == 9 && g.vals[0] == 0x01020304 &&
                    g.vals[i] == 0x01020304);
  return 0;
}
