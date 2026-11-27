// This file is part of the SV-Benchmarks collection of verification tasks:
// https://gitlab.com/sosy-lab/benchmarking/sv-benchmarks
//
// SPDX-FileCopyrightText: 2023 Edoardo Manino
//
// SPDX-License-Identifier: MIT

#include <verifier_functions.h>

#include <math.h>

int main() /* check_domain */
{
	float x = __VERIFIER_nondet_float();
	
	__VERIFIER_assume(isless(x, 0.0f));
	
	float y = logf(x);
	
	__VERIFIER_assert(isnan(y)); /* Expected result: verification successful */

    return 0;
}
// Imported from ESBMC regression/floats-regression/nn-log_0_safe
// Needs: -I .
// RUN: -I . --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
