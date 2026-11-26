#include <assert.h>
typedef struct {
  int v; // FAM needs at least one variable
  int arr[]; // Array of size 0
} FAM;


main() {
  int value = 42;
  FAM *ptr = (FAM*) malloc(sizeof(FAM));
  ptr->v = value;
  FAM deref = *ptr;
  assert(deref.v != value); // out-of-bounds (0 sized array)
  free(ptr);
}
// Imported from ESBMC regression/esbmc/fam_false_3
// RUN: --force-malloc-success -Wno-error=implicit-function-declaration -Wno-error=implicit-int
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
