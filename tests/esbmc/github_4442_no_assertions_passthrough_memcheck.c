/* Regression for #4442 review --- boundary pin for the passthrough test.
 *
 * Same body as github_4442_no_assertions_passthrough, but with
 * --memory-leak-check enabled.  In that mode the __assert_fail
 * noreturn truncation must fire (ASSUME false), making the NULL
 * dereference unreachable.  Pairs with the passthrough test to
 * ensure the --memory-leak-check gate works in both directions.
 *
 * Expected: VERIFICATION SUCCESSFUL.
 */

extern void __assert_fail(const char *, const char *, unsigned int,
                          const char *)
    __attribute__((__nothrow__)) __attribute__((__noreturn__));

extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  int *p = (int *)0;
  if (x == 42)
  {
    __assert_fail("0", __FILE__, __LINE__, __func__);
    return *p;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4442_no_assertions_passthrough_memcheck
// Needs: --memory-leak-check --no-abnormal-memory-leak --no-assertions
// RUN: --memory-leak-check --no-abnormal-memory-leak --no-assertions --incremental-bmc --k-step 2 --64
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
