/* This test is just to check if the no-unroll
 * option is working properly 
 * If the loop is unrolled this should give find the error
 *    with a small K
 * 
 * If it is isn't, it will not be able to find the error.
*/ 

int main() {
    int arr[500];

    for(int i = 0; i < 500; i++) arr[i] = i;
    for(int i = 0; i < 500; i++) __VERIFIER_assert(arr[i] != i); // This is a violation
    
    return 0;
}
// Imported from ESBMC regression/esbmc/loop_unroll_kind_true
// Needs: --k-induction
// RUN: --k-induction
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
