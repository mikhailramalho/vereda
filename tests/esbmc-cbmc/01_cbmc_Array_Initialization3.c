unsigned char array[2][2] = {
  {1, 2},
  {3, 4}
};

void main(void)
{
  assert(array[0][1] ==2);
  assert(array[1][0] ==3);    // returned false in this case
}
// Imported from ESBMC regression/cbmc/01_cbmc_Array_Initialization3
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
