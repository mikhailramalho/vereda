
#include "file1.h"

 int main()

{

    func1(32);

}
// Imported from ESBMC regression/esbmc/github_902-extern
// RUN: --no-incremental-bmc
// FILES: file1.c
// EXPECT: ^VERIFICATION SUCCESSFUL$
