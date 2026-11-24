#include <string.h>

typedef struct __attribute__((__packed__)) {
	char a;
	short b;
} P;

static P q[17];

short fun2(P *p, size_t i)
{
	__VERIFIER_assume(i < 17);
	return p[i].b;
}
// Imported from ESBMC regression/esbmc/packed-2
// RUN: --entry fun2 --no-pointer-check
// EXPECT: ^WARNING: not checking alignment .* packed struct .*P.*$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^WARNING: not checking alignment .* packed struct .*P.*$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
