#include <stdbool.h>

// Variables
bool isFirstCycle = true;
bool property = true;

// Main
void main() {
	goto loop_start;
	loop_start: {
		goto monitor_evaluation;
	}
	
	property_check: {
		// Does not find the violation
		__VERIFIER_assert(property); // assertion error
		goto loop_start;	
	}
		
	monitor_evaluation: {
		// Property becomes false at second iteration
		property = (isFirstCycle ? (true) : (false)) ;
		isFirstCycle = false;
		// Violation found if uncommented
		//__VERIFIER_assert(property, "assertion error");
		goto property_check;	
	}
	
	/* If we put the property_check under the monitor_evaluation the violation is also found
	property_check: {
		// Violation found
		__VERIFIER_assert(property, "assertion error");
		goto loop_start;	
	}
	*/
	return;
}

// Imported from ESBMC regression/k-induction/github_1735_2
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
