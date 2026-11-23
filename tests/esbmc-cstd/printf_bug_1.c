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
// Needs: REQUIRES arch32
// RUN: --overflow-check --32
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdereference failure: NULL pointer\b
