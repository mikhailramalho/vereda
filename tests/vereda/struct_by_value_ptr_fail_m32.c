// struct_by_value_ptr_fail.c on i386.
// RUN: -m32 --unwind 5 -DFAIL
// EXPECT: ^VERIFICATION FAILED$
#include "struct_by_value_ptr.c"
