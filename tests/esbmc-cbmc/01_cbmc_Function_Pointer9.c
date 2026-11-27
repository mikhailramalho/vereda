#include <assert.h>

struct gendisk {
  void (*request_fn)(int);
};

struct my_gendisk {
  struct gendisk gd;
};

void test_func(int i)
{
  assert(0);

  return;
}

int main()
{
  struct my_gendisk mydisk;

  mydisk.gd.request_fn = test_func;

  (* mydisk.gd.request_fn)(10);

  return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Function_Pointer9
// EXPECT: ^VERIFICATION FAILED$
