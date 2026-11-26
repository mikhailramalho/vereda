int main() {
        int foo;
        foo = 42;
        int asd = foo + 10;
        foo = 15;
        __VERIFIER_assert(0); // bar
        return 0;
}
// Imported from ESBMC regression/esbmc/github_666-id
// Needs: c:no_slice.c@21@F@main@foo?1!0&0#3
// RUN: c:no_slice.c@21@F@main@foo?1!0&0#3
// MODE: FUTURE
// EXPECT: \bfoo = 15\b
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
