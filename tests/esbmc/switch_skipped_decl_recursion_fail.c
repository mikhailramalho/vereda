#include <assert.h>

/* The switch jumps past y's declaration, but each call of f still has its
 * own y (C11 6.2.4p6): the outer y is still 0 after the inner call. */
void f(int k)
{
  switch (k)
  {
    int y;
  case 0:
  case 1:
    y = k;
    if (k == 0)
    {
      f(1);
      assert(y == 1);
    }
  }
}

int main()
{
  f(0);
}
// Imported from ESBMC regression/esbmc/switch_skipped_decl_recursion_fail
// EXPECT: ^VERIFICATION FAILED$
