#include <stdlib.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
    float tmp;
    scanf("%13f", &tmp);
    printf("%f",tmp);
}
// Imported from ESBMC regression/cstd/scanf_float_bug_2
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict SUCCESSFUL
