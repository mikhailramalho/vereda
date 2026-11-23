typedef struct {
  int v; // FAM needs at least one variable
  int arr[]; // Array of size 0
} FAM;

int main() {
  FAM F = {1, {}};
  F.arr[10] = 7; // out-of-bounds
}
// Imported from ESBMC regression/esbmc/fam_false_0
// Needs: --compact-trace
// RUN: --compact-trace
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
