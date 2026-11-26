
typedef struct {
  int v; // FAM needs at least one variable
  int arr[]; // Array of size 0
} FAM;


main() {
  FAM *ptr = (FAM*) malloc(sizeof(FAM) + sizeof(int)*3);
  ptr->arr[3] = 42; // out-of-bounds
  free(ptr);
}
// Imported from ESBMC regression/esbmc/fam_false_1
// RUN: --force-malloc-success -Wno-error=implicit-function-declaration -Wno-error=implicit-int
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
