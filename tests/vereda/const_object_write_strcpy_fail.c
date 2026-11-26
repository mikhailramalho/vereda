// Writing into a const global via strcpy is undefined (C11 6.7.3p6).
// InstCombine must not lower and delete the write before the walker checks it.
// EXPECT: ^  dereference failure: write access to const object `buf'$
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>

const char buf[4] = "abc";

int main(void) {
  strcpy((char *)buf, "d");
  return 0;
}
