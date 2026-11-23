unsigned __VERIFIER_nondet_uint(void);
unsigned inc(unsigned v) { return v + 1; }
unsigned find(unsigned n) {
  unsigned i = 0;
  while (i < n) {
    if (i == 7) return i;
    i = inc(i);
    for (unsigned j = 0; j < 3; j++) {
      if (j == i) break;
      if (j == 2) continue;
      i = i + 0;
    }
  }
  return n;
}
int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  unsigned r = find(n);
  __VERIFIER_assert(r <= n || r == 7); // r
  return 0;
}
// Imported from ESBMC regression/goto2lean/unsupported_call_in_loop
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: unsupported call to inc inside a loop
// Vereda: verdict none
