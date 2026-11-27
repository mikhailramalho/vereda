typedef unsigned long ulong;
extern ulong __VERIFIER_nondet_ulong();

int main()
{
  ulong content_len = __VERIFIER_nondet_ulong();
  __builtin_umull_overflow(content_len, 10UL, &content_len);
}
// Imported from ESBMC regression/esbmc/builtin_arith_overflow_3
// EXPECT: ^VERIFICATION SUCCESSFUL$
