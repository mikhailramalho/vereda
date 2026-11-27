union foo { int i; double d; };
union foo FOO;

int main() {
    int x = __VERIFIER_nondet_int();
    __VERIFIER_assume(x > 10);

    FOO = (union foo) x;
    __VERIFIER_assert(FOO.i > 10); // int value should initialize union foo properly
    return 0;
}
// Imported from ESBMC regression/esbmc/to_union_0
// EXPECT: ^VERIFICATION SUCCESSFUL$
