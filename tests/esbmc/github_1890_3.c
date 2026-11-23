#include <assert.h>

void func()
{
   for (int i = 0; i <= 1; i++)
      assert(0 && "1");
}
void func2()
{

   assert(0 && "2");
}
int main()
{
   func();
   int x = 0;
   10 / x;
   func2();
}
// Imported from ESBMC regression/esbmc/github_1890_3
// RUN: --multi-property --no-unwinding-assertions --unwind 3
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdivision by zero\b
// Vereda: unmet EXPECT: \bdivision by zero\b
// MODE: FUTURE
// Vereda: verdict FAILED
