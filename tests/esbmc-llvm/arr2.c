//#include <assert.h>

int array2[10] = {1, 2, 3};

void fun(int initval)
{
  float array[] = {1, 2, 3};
  float array1[] = {1, 2, 3};
  int array3[10] = {1, 2, 3};
  double array4[initval];

  int x = array2[9];
  assert(x == 0);
  assert(array4[0] == 0);
}

int main()
{
  fun(2);
  return 0;
}
// Imported from ESBMC regression/llvm/arr2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
