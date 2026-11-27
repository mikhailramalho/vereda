union foo { int i; double d; };
union foo FOO;

int bar(union foo f) {
    return f.i;
}

int main() {
    int x = __VERIFIER_nondet_int();
    __VERIFIER_assume(x > 10);

    int v = bar((union foo) x);
    __VERIFIER_assert(v > 10); // int value should initialize union foo properly
    return 0;
}
// Imported from ESBMC regression/esbmc/to_union_2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
