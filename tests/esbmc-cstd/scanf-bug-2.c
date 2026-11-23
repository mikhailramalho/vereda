#include <stdlib.h>
int main(int argc, char *argv[])
{
    int* ptr = (int*) malloc(sizeof(int));
    scanf("%12d", ptr);  // input 12345678901 overflow
    printf("%d",*ptr);   // output --539222987
}
// Imported from ESBMC regression/cstd/scanf-bug-2
// RUN: --overflow-check -m64
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: KNOWNBUG test passed unexpectedly; promote it to CORE
// MODE: CORE
