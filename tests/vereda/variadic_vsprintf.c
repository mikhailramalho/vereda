// vsprintf has no model, and a bodyless call writes nothing through its
// buffer, so the overflow here would go unseen: unsupported.
// RUN: -m64
// EXPECT: ^VERIFICATION UNKNOWN: unsupported: call to vsprintf
#include <stdarg.h>
#include <stdio.h>

void put(char *b, const char *f, ...) {
  va_list ap;
  va_start(ap, f);
  vsprintf(b, f, ap);
  va_end(ap);
}

int main(void) {
  char b[4];
  put(b, "%s", "hello");
  return 0;
}
