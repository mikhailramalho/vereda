// variadic_count_false.c for i386.
// RUN: -m32 --unwind 6
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 24  __VERIFIER_assert$
// EXPECT: ^VERIFICATION FAILED$
#include "variadic_count_false.c"
