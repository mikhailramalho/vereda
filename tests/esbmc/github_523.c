#include <stdio.h>

int x = 5;
int secret_key = 10;

void access_memory() 
//@requires true;
//@ensures true;
{
    int *p = &x;
    p++;//disable overflow 
    
    printf("The value of secret value is: %d\n", *p);

}

int main()
//@requires true;
//@ensures true;
{
    access_memory();

    return 0;
}
// Imported from ESBMC regression/esbmc/github_523
// RUN: --no-incremental-bmc
// EXPECT: dereference failure: Access to object out of bounds
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: dereference failure: Access to object out of bounds
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
