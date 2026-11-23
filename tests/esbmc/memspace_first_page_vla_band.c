#include <assert.h>

/* An array too large to place above the first page still gets an address. */
int main(void)
{
  unsigned long n = __VERIFIER_nondet_ulong();
  if (n > 18446744073709551615ul - 4000 && n <= 18446744073709551615ul - 16)
  {
    char a[n];
    a[0] = 1;
    assert((unsigned long)a != 0);
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/memspace_first_page_vla_band
// Needs: --reserve-first-page
// RUN: --no-vla-size-check --no-bounds-check --reserve-first-page
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
