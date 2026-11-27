#include <stdlib.h>
int main(int argc, char *argv[])
{
    int* ptr = (int*) malloc(sizeof(int));
    scanf("%12d", ptr);  // input 12345678901 overflow
    printf("%d",*ptr);   // output --539222987
}
// Imported from ESBMC regression/cstd/scanf-bug-2
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check -m64 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
