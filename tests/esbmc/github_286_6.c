int main(void) {
    int* p;
    for (int i = 0; i < 10; i++) {
        int a[10];
        p = a;
        p[0] = 1;
    }
    p[0] = 2;

    return 0;
}
// Imported from ESBMC regression/esbmc/github_286_6
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
