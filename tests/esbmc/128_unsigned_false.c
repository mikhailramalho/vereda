#include <assert.h>
int main() {
    unsigned __int128 Z = -1;
    unsigned __int128 X = 1;
    X = X >> 70;
    assert(Z < X);
}
// Imported from ESBMC regression/esbmc/128_unsigned_false
// EXPECT: ^VERIFICATION FAILED$
