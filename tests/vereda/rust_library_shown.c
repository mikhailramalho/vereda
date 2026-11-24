// The flag restores Rust standard-library assignments.
// RUN: --show-library-states
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: value = 5;
// EXPECT: visible = 7;
// EXPECT: internal = 5;
#include "rust_library_states.h"
