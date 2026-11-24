// Loops the pass must leave alone; only the last one is summarised.
int __VERIFIER_nondet_int();
void g(void) {}
int a[10], b[10];
int main()
{
  int n = __VERIFIER_nondet_int();
  int i = 0;
  do { a[i] = 1; i++; } while (i < 10);
  for (int k = 10; k > 0; k--) a[k - 1] = 2;
  for (float f = 0; f < 3; f++) a[0] = 3;
  i = 0; while (i < 3) { i++; g(); }
  for (int k = 0; k < 10; k++) { if (k == 5) break; a[k] = 5; }
  for (int k = 0; k < 10; k++) { a[k] = 6; b[k] = 6; }
  int *p = b;
  for (int k = 0; k < 10; k++) a[k] = p[k];
  for (int k = 0; k < a[0]; k++) a[k] = 7;
  if (n) goto in;
  for (i = 0; i < 10; i++) { in: b[i] = 8; }
  for (long k = 0; k < 10; k++) a[k] = 9;
  __VERIFIER_assert(a[3] == 9); // a[3]
  return 0;
}
// Imported from ESBMC regression/esbmc/array_loop_summary_rejected
// RUN: --unwind 12 --no-unwinding-assertions
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^Summarised 1 array loop\(s\)$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
