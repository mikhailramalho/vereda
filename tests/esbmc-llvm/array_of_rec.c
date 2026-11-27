
int arr1[5];
int arr2[5][5];
int arr3[5][5][5];
int arr4[5][5][5][5];
int arr5[5][5][5][5][5];
int arr6[5][5][5][5][5][5];

int main()
{
  return 0;
}
// Imported from ESBMC regression/llvm/array_of_rec
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
