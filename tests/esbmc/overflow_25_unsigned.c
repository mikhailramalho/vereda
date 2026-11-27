#include <stdio.h>
#include <assert.h>

int main(int argc, char **argv)
{
unsigned int delta = 0;
unsigned int i=5;
unsigned int j=3;

delta = j - i;
printf("\nDelta %u", delta);

return 0;
}
// Imported from ESBMC regression/esbmc/overflow_25_unsigned
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check
// EXPECT: ^VERIFICATION FAILED$
