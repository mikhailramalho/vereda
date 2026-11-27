#include <assert.h>
#include <uchar.h>

int main()
{
	const char16_t *s = u"\x1234";
	assert(s[0] == u'\x1234');
	assert(s[1] == u'\0');
}
// Imported from ESBMC regression/esbmc/char16-lit-no-simp
// Needs: REQUIRES uchar_h
// RUN: --no-simplify --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
