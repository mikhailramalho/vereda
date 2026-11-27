#include <assert.h>
extern void __builtin();

int main()
{
  __builtin();

  assert(0);
}
// Imported from ESBMC regression/esbmc/builtin_missing_body_wrong_pc
// EXPECT: ^VERIFICATION FAILED
