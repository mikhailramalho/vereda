#include <stdio.h>
#include <assert.h>
int main()
{
	char *s = "abcde123415";
	int x = printf("%.*s\n", 2, s); 
    int y = printf("%.2s\n", s);
    assert(x == 3);
    assert(y == 3);   
}
// Imported from ESBMC regression/cstd/printf_bug_1
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check -m32
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdereference failure: NULL pointer\b
// Vereda: verdict FAILED
