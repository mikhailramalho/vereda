// Checks if relation operations don't throw errors anymore
int main(void) {
    int* p = (int*) malloc(sizeof(int));
    int q;

    if(p < &q) return 1;
    return 0;
}
// Imported from ESBMC regression/esbmc/no_pointer_check_1
// EXPECT: ^VERIFICATION SUCCESSFUL$
