#include <assert.h>
#include <stdlib.h>

void unknown_method(int *a);
int a = 1;

int main()
{
  int *p = &a;

  unknown_method(p);

  *p; // access invalid pointer
}
// Imported from ESBMC regression/esbmc/unknown_method_call
// Needs: --unknown-method-args-check
// RUN: --unknown-method-args-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
