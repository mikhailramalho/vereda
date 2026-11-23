#include <stdio.h>
int main() {
    int n;
    printf("Enter an integer: ");
    scanf("%1000000000000000000000000d", &n);
    printf("%d\n",n);
    return 0;
}
// Imported from ESBMC regression/cstd/scanf_bigint_1
// RUN: --overflow-check
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
