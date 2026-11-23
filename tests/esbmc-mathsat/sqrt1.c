#include <assert.h>
float sqrt1(const float x)  
{
  union
  {
    int i;
    float x;
  } u;
  u.x = x;
  u.i = (1<<29) + (u.i >> 1) - (1<<22); 
  return u.x;
} 
float __VERIFIER_nondet_float(); 
int main()
{
  float sq;
  float val;
  val = __VERIFIER_nondet_float(); 
  __VERIFIER_assume(val>0 && val<20);
  sq = sqrt1(val);
  assert(sq < 5);
  return 0;
}


// Imported from ESBMC regression/mathsat/sqrt1
// Needs: --mathsat
// RUN: --floatbv --mathsat
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
