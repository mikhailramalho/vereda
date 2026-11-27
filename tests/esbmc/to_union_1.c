union foo { int i; double d; };
union foo FOO;

int main() {
    double x = __VERIFIER_nondet_double();
    __VERIFIER_assume(x > 10);

    FOO = (union foo) x;
    __VERIFIER_assert(FOO.d > 10); // double value should initialize union foo properly
    return 0;
}
// Imported from ESBMC regression/esbmc/to_union_1
// EXPECT: ^VERIFICATION SUCCESSFUL$
