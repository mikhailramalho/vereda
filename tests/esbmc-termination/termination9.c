/*
 * Date: 2014-06-22
 * Author: heizmann@informatik.uni-freiburg.de
 */
 
int main() {
	int* x0 = __builtin_alloca(sizeof(int));
	int* x1 = __builtin_alloca(sizeof(int));
	int* x2 = __builtin_alloca(sizeof(int));
	int* x3 = __builtin_alloca(sizeof(int));
	*x0 = 0;
	*x1 = 0;
	*x2 = 0;
	*x3 = 0;
	while ( *x3 == 0 ) {
		if (*x0 == 0) {
			*x0 = 1;
		} else {
			*x0 = 0;
			if (*x1 == 0) {
				*x1 = 1;
			} else {
				*x1 = 0;
				if (*x2 == 0) {
					*x2 = 1;
				} else {
					*x2 = 0;
					*x3 = 1;
				}
			}
		}
	}
	return 0;
}
// Imported from ESBMC regression/termination/termination9
// Needs: --state-hashing --add-symex-value-sets --k-step 2 --unlimited-k-steps --no-assertions --termination --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --floatbv --unlimited-k-steps --no-vla-size-check --64 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
