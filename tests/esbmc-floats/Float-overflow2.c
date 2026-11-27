int main()
{
  float f=1;

  for(int i=0; i<128; i++)
    f*=2; // overflows in the last iteration
}
// Imported from ESBMC regression/floats/Float-overflow2
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
