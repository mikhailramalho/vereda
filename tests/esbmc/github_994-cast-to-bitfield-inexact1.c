#include <assert.h>

union U { unsigned raw : 24; struct { unsigned x : 16, y : 16; }; };

unsigned f();

int main() {
	unsigned x = f();
	assert(((union U)x).raw == (x & 0xffffff));
}
// Imported from ESBMC regression/esbmc/github_994-cast-to-bitfield-inexact1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
