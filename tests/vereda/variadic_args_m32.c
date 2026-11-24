// variadic_args.c for i386, where every argument goes on the stack and
// va_list is a pointer to the next one.
// RUN: -m32
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include "variadic_args.c"
