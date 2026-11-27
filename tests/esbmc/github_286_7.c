extern int printf ( const char * format, ... );

int *foo2(void)
{
    int arr[1024];
    arr[194] = 13;
    return arr + 1;
}

int *foo(void)
{
    int arr[123];
    return foo2();
}

int main(void) {
    int *a = foo();
    printf("%d\n", *a);
    return 0;
}
// Imported from ESBMC regression/esbmc/github_286_7
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
