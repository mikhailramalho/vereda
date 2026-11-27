#include "1.h"

void foo(char, int);

int main()
{

}
// Imported from ESBMC regression/llvm/function
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
