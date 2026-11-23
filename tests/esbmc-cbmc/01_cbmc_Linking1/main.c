// to screw up the anon-counter

struct
{
  int abc;
};

#include "module.h"

anon_struct a_struct;

int i;

int main()
{
  assert(i==1);
  
  f();
  
  assert(i==2);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Linking1
// FILES: module.c
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
