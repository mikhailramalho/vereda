// A conditional write of a nondet value: two written elements may differ.
// EXPECT: ^\[INFO\] Summarised 1 array loop\(s\)$
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
#define N 3000
unsigned char a[N];
int main(void) {
  for (unsigned i = 0; i < N; i++)
    if (i % 3 == 1)
      a[i] = __VERIFIER_nondet_int();
  __VERIFIER_assert(a[4] == a[7]);
  return 0;
}
