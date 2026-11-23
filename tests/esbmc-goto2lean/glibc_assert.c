/* glibc's assert expansion, written out so every host sees its leading
 * `(void) sizeof (...)` expression statement. */
extern void
__assert_fail(const char *, const char *, unsigned int, const char *);
#define ASSERT_GLIBC(expr)                                                     \
  ((void)sizeof((expr) ? 1 : 0), __extension__({                               \
     if (expr)                                                                 \
       ;                                                                       \
     else                                                                      \
       __assert_fail(#expr, __FILE__, __LINE__, __func__);                     \
   }))
unsigned __VERIFIER_nondet_uint(void);
unsigned mid(unsigned lo, unsigned hi)
{
  return (lo + hi) / 2;
}
int main(void)
{
  unsigned lo = __VERIFIER_nondet_uint(), hi = __VERIFIER_nondet_uint();
  __VERIFIER_assume(lo <= hi);
  unsigned m = mid(lo, hi);
  ASSERT_GLIBC(lo <= m && m <= hi);
  return 0;
}
// Imported from ESBMC regression/goto2lean/glibc_assert
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^  \(lo ≤ hi\) →$
// EXPECT: ^  \(\(lo ≤ m\) ∧ \(m ≤ hi\)\) ∧ \($
// EXPECT: ^  all_goals bv_decide$
// Vereda: verdict none
