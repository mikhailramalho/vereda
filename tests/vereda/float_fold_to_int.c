// Conversions of constant floats to integers fold when the value, truncated
// toward zero, is in range; a fraction above -1 converts to unsigned 0.
// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
double in[] = {2147483647.9, -2147483648.9, -0.9, 4294967295.5, -2.5, 0x1.fffffffffffffp62};
int main(void) {
  __VERIFIER_assert((int)in[0] == 2147483647 && (int)in[1] == -2147483647 - 1);
  __VERIFIER_assert((unsigned)in[2] == 0 && (unsigned)in[3] == 4294967295u);
  __VERIFIER_assert((int)in[2] == 0 && (signed char)in[4] == -2 && (_Bool)in[2]);
  __VERIFIER_assert((long)in[5] == 0x7ffffffffffffc00L && (unsigned long)in[5] == 0x7ffffffffffffc00UL);
  return 0;
}
