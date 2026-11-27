#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char name[20] = "";

    __VERIFIER_assume(argc == 10);
    __VERIFIER_assert(0); // 0
}
// Imported from ESBMC regression/esbmc/github_2312_4
// EXPECT: ^VERIFICATION FAILED$
