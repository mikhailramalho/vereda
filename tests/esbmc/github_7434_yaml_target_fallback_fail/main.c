/* Regression for #7434: see body.h. */
#include "body.h"
// Imported from ESBMC regression/esbmc/github_7434_yaml_target_fallback_fail
// Needs: --witness-output-yaml -
// RUN: --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: type: "target"
// EXPECT: file_name: "[^"]*main\.c"
// EXPECT: (?s)\A(?!.*file_name: "[^"]*body\.h")
// Vereda: verdict none
