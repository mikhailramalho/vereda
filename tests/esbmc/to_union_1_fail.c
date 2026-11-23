union foo { int i; double d; };
union foo FOO;

int main() {
    FOO = (union foo) FLT_MAX;
    __VERIFIER_assert(0); // this assertion shouldn't be reached
    return 0;
}
// Imported from ESBMC regression/esbmc/to_union_1_fail
// EXPECT: ^ERROR: PARSING ERROR$
// Vereda: unmet EXPECT: ^ERROR: PARSING ERROR$
// MODE: FUTURE
// Vereda: verdict none
