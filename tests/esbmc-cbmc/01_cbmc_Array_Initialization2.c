int arr[2] = {0,0};

int main()
{
  unsigned place = __VERIFIER_nondet_int();
  __VERIFIER_assume(place<2);
  assert(arr[place]==0);
  return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Array_Initialization2
// EXPECT: ^VERIFICATION SUCCESSFUL$
