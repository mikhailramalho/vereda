#include <assert.h>
#include <uchar.h>

int main()
{
	const char32_t *s = U"\x12345";
	char32_t c0 = s[0];
	char32_t c1 = s[1];
	assert(s[0] == U'\x12345');
	assert(s[1] == U'\0');
}
// Imported from ESBMC regression/esbmc/char32-lit-no-simp
// Needs: REQUIRES uchar_h
// RUN: --no-simplify --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
