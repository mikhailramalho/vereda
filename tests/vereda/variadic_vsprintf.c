// vsprintf and vsnprintf read their arguments through the caller's
// va_list, here the variadic arguments of put.
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
extern void __VERIFIER_assert(int);

int put(char *b, size_t n, const char *f, ...) {
  va_list ap, aq;
  va_start(ap, f);
  va_copy(aq, ap);
  int r = vsnprintf(b, n, f, ap);
  char whole[16];
  __VERIFIER_assert(vsprintf(whole, f, aq) == r && strncmp(b, whole, n - 1) == 0);
  va_end(aq);
  va_end(ap);
  return r;
}

int main(void) {
  char b[4];
  __VERIFIER_assert(put(b, sizeof b, "%s%ld", "he", 123456789012L) == 14);
  __VERIFIER_assert(strcmp(b, "he1") == 0);
  return 0;
}
