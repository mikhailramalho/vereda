unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned c = __VERIFIER_nondet_uint(), d = __VERIFIER_nondet_uint();
  if (c) {
    if (d) goto L;
    c = 1;
  }
L:
  __VERIFIER_assert(c != 2); // c
  return 0;
}
// Imported from ESBMC regression/goto2lean/goto_to_branch_end
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^  \(\(c ≠ 0#32\) →\n    \(\(d ≠ 0#32\) →\n      join_1 c\) ∧\n    \(¬\(d ≠ 0#32\) →\n    let c := \(1#32\);\n    join_1 c\)\)$
// Vereda: verdict none
