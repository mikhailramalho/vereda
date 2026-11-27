#include <assert.h>
int main() {
    int granules[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    unsigned long int offset = (&granules[0] + 6) - &granules[0];
    assert(offset < 8);
    return 0;
}
// Imported from ESBMC regression/esbmc/github_1401
// EXPECT: ^VERIFICATION SUCCESSFUL$
