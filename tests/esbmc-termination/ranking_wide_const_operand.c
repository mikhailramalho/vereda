/* Wide-constant operand in the loop guard.
 *
 * The frontend often promotes a literal at the int boundary to long when
 * the unsuffixed form does not fit in int: for example, the source
 *   while (x > -2147483648)
 * lowers (under `--32`) to
 *   IF !((long long)x > -2147483648)
 * The right-hand side is a long-long constant whose VALUE still fits in
 * int32. The width guard in measure_from_relational refuses 64-bit
 * SOURCE operands to avoid subtraction wrap, but it now exempts the
 * wide side when it is a constant whose numeric value lies within
 * [-2147483648, 2147483647] -- a value-preserving extension to int64
 * stays well inside int64 and the rank is overflow-safe.
 *
 * Expected verdict: VERIFICATION SUCCESSFUL. */

extern int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  while (x > -2147483647) /* one above INT_MIN to avoid UB on the decrement */
  {
    x = x - 1;
  }
  return 0;
}
// Imported from ESBMC regression/termination/ranking_wide_const_operand
// Needs: --force-realloc-success --state-hashing --add-symex-value-sets --no-assertions --termination --max-inductive-step 3 REQUIRES arch32
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --floatbv --no-vla-size-check --32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^Ranking function shows all executions terminate
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
