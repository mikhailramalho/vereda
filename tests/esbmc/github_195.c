int main(void)
{
    unsigned int b;
    unsigned int a = 0;
    int table[2][2]  = {{1,0}, {1,0}};

    if (__VERIFIER_nondet_uint() == 0) {
        a = 1;
    } else {
        __VERIFIER_assume(0);
    }
    __VERIFIER_assert(table[1][0]) /* FOO */; /* Assertion ok */
    __VERIFIER_assert(a == 1) /* FOO */; /* Assertion ok */
    b = table[a][0];
    __VERIFIER_assert(b) /* ERROR */; /* Assertion violated */
    __VERIFIER_assert(table[a][0]) /* ERROR */; /* arrays bound violated */
    return (0);
}
// Imported from ESBMC regression/esbmc/github_195
// EXPECT: ^VERIFICATION SUCCESSFUL$
