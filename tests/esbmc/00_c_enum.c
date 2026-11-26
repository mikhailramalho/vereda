typedef enum {
	fail,
	notfail,
	alsonotfail
} enumtype;

int
main()
{
	enumtype val;

	if (__VERIFIER_nondet_bool()) {
		val = notfail;
	} else {
		val = alsonotfail;
	}

	assert(val != fail);
	return 0;
}
// Imported from ESBMC regression/esbmc/00_c_enum
// RUN: --no-slice -Wno-error=implicit-function-declaration
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
