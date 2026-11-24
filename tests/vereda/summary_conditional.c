// A conditional write of a nondet value: the summary writes element n
// where iteration n would, a fresh value each, and leaves the others.
// EXPECT: ^Summarised 1 array loop\(s\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
#define N 3000
unsigned char a[N];
int main(void) {
  for (unsigned i = 0; i < N; i++)
    if (i % 3 == 1)
      a[i] = __VERIFIER_nondet_int();
  unsigned x = __VERIFIER_nondet_uint();
  if (x < N && x % 3 != 1)
    __VERIFIER_assert(a[x] == 0);
  return 0;
}
