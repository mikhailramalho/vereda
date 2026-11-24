/* slightly adapted from <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=36566> */

struct S
{
    short   s;
} __attribute__((aligned(8), packed));

void fun3(struct S *s)
{
	/* KNOWNBUG: this should not warn about not checking the alignment
	 * and actually check the alignment as member `S::s` is actually
	 * known to be aligned. */
	s->s;
}
// Imported from ESBMC regression/esbmc/packed-3
// RUN: --entry fun3 --no-pointer-check
// EXPECT: ^WARNING: not checking alignment .* packed struct struct S$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^WARNING: not checking alignment .* packed struct struct S$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
