// RUN: -m32 --unwind 20001 --property unreach-call
// RUN: -m64 --unwind 20001 --property unreach-call
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: unwinding assertion loop 1
// Stop at the second loop bound to measure construction without a large solve.

extern short __VERIFIER_nondet_short(void);
extern unsigned char __VERIFIER_nondet_uchar(void);
extern void reach_error(void);

int main(void) {
  int a[20000], b[20000];
  for (int i = 0; i < 20000; ++i) {
    a[i] = i;
    b[i] = i;
  }
  for (int i = 0; i < 20001; ++i)
    if (i < 20000 && __VERIFIER_nondet_short()) {
      int k = __VERIFIER_nondet_uchar();
      a[i] = k;
      b[i] = k * k;
    }
  if (a[19999] != b[19999] && b[19999] != a[19999] * a[19999])
    reach_error();
}
