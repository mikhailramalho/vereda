// Writing into a string literal via strcpy is undefined (C11 6.4.5p7).
// InstCombine must not turn the call into a memcpy and delete the write
// before the walker checks it.
// EXPECT: ^  dereference failure: write access to string constant$
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>

int main(void) {
  strcpy((char *)"abc", "d");
  return 0;
}
