// struct_by_value_ptr.c with an assertion the value read through the
// returned pointer breaks.
// RUN: -m64 --unwind 5 -DFAIL
// EXPECT: ^VERIFICATION FAILED$
#include "struct_by_value_ptr.c"
