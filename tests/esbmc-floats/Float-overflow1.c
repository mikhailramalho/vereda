int main()
{
  float f=1;

  for(int i=0; i<127; i++)
    f*=2; // should not overflow
}
// Imported from ESBMC regression/floats/Float-overflow1
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
