#include <assert.h>

int main(void)
{
  double arr[4] = {0.0, 0.0, 0.0, 0.0};
  arr[2] = -0.0;
  assert(__builtin_signbit(arr[2]) == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7321_fail
// EXPECT: ^VERIFICATION FAILED$
