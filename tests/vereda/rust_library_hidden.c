// Rust standard-library assignments are hidden while the caller is retained.
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: value = 5;
// EXPECT: visible = 7;
// EXPECT-NOT: internal =
#include "rust_library_states.h"
