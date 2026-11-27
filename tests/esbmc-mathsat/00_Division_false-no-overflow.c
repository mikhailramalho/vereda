// Author: heizmann@informatik.uni-freiburg.de
// Date: 2015-09-01
//
// We assume sizeof(int)=4.

#include <stdio.h>

int main() {
	int minInt = -2147483647 - 1;
	int x = (minInt / -1 ) - 1;
	printf("%d\n", x);
	return 0;
}
// Imported from ESBMC regression/mathsat/00_Division_false-no-overflow
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check
// EXPECT: ^VERIFICATION FAILED$
