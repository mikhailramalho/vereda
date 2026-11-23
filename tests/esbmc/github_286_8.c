int main(void) {
    int* p;
    if(1) {
        int a[10];
        p = a;
    }
    p[0] = 1;
}
// Imported from ESBMC regression/esbmc/github_286_8
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
