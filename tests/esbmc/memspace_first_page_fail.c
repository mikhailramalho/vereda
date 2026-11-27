#include <assert.h>

/* The first address past the first page is a valid object address. */
char buf[16];

int main(void)
{
  assert((unsigned long)buf != 4096);
  return 0;
}
// Imported from ESBMC regression/esbmc/memspace_first_page_fail
// Needs: --reserve-first-page
// RUN: --reserve-first-page --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
