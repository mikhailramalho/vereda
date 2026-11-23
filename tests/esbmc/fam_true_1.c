
typedef struct {
  int v; // FAM needs at least one variable
  int arr[]; // Array of size 0
} FAM;

main() {
  FAM *ptr = (FAM*) malloc(sizeof(FAM) + sizeof(int)*3);
  ptr->arr[2] = 42; // out-of-bounds
  free(ptr);
}
// Imported from ESBMC regression/esbmc/fam_true_1
// Needs: --compact-trace
// RUN: --compact-trace  --force-malloc-success -Wno-error=implicit-function-declaration -Wno-error=implicit-int
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
