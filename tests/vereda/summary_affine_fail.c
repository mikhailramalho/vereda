// A loop that writes an affine value of its index to every element is
// replaced by its effect, which keeps the element the claim gets wrong.
// EXPECT: ^\[INFO\] Summarised 1 array loop\(s\)$
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern unsigned __VERIFIER_nondet_uint(void);
#define N 100000
int a[N];
int main(void) {
  for (int i = 0; i < N; i++)
    a[i] = 3 * i + 1;
  unsigned x = __VERIFIER_nondet_uint();
  if (x < N)
    __VERIFIER_assert(a[x] != 3 * 77777 + 1);
  return 0;
}
