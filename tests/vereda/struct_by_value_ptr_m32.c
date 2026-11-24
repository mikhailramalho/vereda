// struct_by_value_ptr.c on i386, which returns the struct through a hidden
// pointer and passes its fields as separate arguments.
// RUN: -m32 --unwind 5
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include "struct_by_value_ptr.c"
