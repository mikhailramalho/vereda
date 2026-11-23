int main(void) {
    int* p;
    for(int i = 0; i < 2; i++) {
        int a[10];
        if(i == 0) {
            p = a;
        } else {
            p[0] = 1;
        }
    }
    return 0;
}
// Imported from ESBMC regression/esbmc/github_286_1
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
