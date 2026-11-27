union foo { int i; double d; };
union foo FOO;

int main() {
    char c = __VERIFIER_nondet_char();
    __VERIFIER_assume(c > 10);

    FOO = (union foo) c;
    __VERIFIER_assert(0); // this assertion shouldn't be reached
    return 0;
}
// Imported from ESBMC regression/esbmc/to_union_0_fail
// RUN: --no-incremental-bmc
// EXPECT: ^ERROR: PARSING ERROR$
// Vereda: unmet EXPECT: ^ERROR: PARSING ERROR$
// MODE: FUTURE
// Vereda: verdict none
