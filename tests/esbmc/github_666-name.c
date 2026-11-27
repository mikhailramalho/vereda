int main() {
        int foo;
        foo = 42;
        int asd = foo + 10;
        foo = 15;
        __VERIFIER_assert(0); // bar
        return 0;
}
// Imported from ESBMC regression/esbmc/github_666-name
// Needs: c:no_slice.c@21@F@main@foo
// RUN: c:no_slice.c@21@F@main@foo
// MODE: FUTURE
// EXPECT: \bfoo = 42\b
// EXPECT: \bfoo = 15\b
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
