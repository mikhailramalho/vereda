int main() {
        int foo __attribute__ ((annotate("__ESBMC_no_slice")));
        foo = 42;
        int asd = foo + 10;
        foo = 15;
        __VERIFIER_assert(0); // bar
        return 0;
}
// Imported from ESBMC regression/esbmc/github_666
// RUN: --no-incremental-bmc
// EXPECT: \bfoo = 42\b
// EXPECT: \bfoo = 15\b
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: \bfoo = 42\b
// MODE: FUTURE
// Vereda: verdict UNKNOWN
