#include <assert.h>
#include <stdlib.h>
typedef struct {
  int v; // FAM needs at least one variable
  int arr[]; // Array of size 0
} FAM;


main() {
  FAM *ptr = (FAM*) malloc(sizeof(FAM));
  assert(ptr->arr != NULL);
  free(ptr);
}
// Imported from ESBMC regression/esbmc/fam_true_2
// Needs: --compact-trace
// RUN: --compact-trace  --force-malloc-success -Wno-error=implicit-int
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
