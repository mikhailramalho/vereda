
typedef struct {
  int v; // FAM needs at least one variable
  int arr[]; // Array of size 0
} FAM;


main() {
  FAM *ptr = (FAM*) malloc(sizeof(FAM));
  ptr->arr[0] = 42; // out-of-bounds (0 sized array)
  free(ptr);
}
// Imported from ESBMC regression/esbmc/fam_false_2
// Needs: --compact-trace
// RUN: --compact-trace  --force-malloc-success -Wno-error=implicit-function-declaration -Wno-error=implicit-int
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: dereference failure: Access to object out of bounds: heap object
// Vereda: verdict none
