#include <assert.h>

int counter = 0;

void increment()
{
  counter = 1;
}

void check()
{
  assert(counter == 0);
}

int main()
{
  void (*fun_ptr)(void);
  int a;
  if (a)
    {
      fun_ptr();
      // This is reachable with no-pointer-check!
      increment();
    }  
  check();
}
// Imported from ESBMC regression/esbmc/interval-analysis-function-ptr-false
// Needs: --interval-analysis
// RUN: --interval-analysis --ub-pointer-check=false
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
