#include <assert.h>
#include <uchar.h>

int main()
{
	const char16_t *s = u"\x1234";
	assert(s[0] == u'\x1234');
	assert(s[1] == u'\0');
}
// Imported from ESBMC regression/esbmc/char16-lit
// Needs: REQUIRES uchar_h
// RUN: 
// EXPECT: ^VERIFICATION SUCCESSFUL$
