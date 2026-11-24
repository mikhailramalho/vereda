#include <assert.h>
#include <stdint.h>

int main()
{
	int64_t data;
	return data == -data;
}
// Imported from ESBMC regression/esbmc/overflow-neg
// RUN: --overflow-check
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: // 10000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
// EXPECT: arithmetic overflow on neg
