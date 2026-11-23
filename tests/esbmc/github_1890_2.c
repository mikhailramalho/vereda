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
    func2();
}
// Imported from ESBMC regression/esbmc/github_1890_2
// Needs: --k-induction
// RUN: --multi-property --k-induction --k-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
