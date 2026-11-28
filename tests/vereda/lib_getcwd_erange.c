// RUN: --unwind 9
// EXPECT: ^VERIFICATION SUCCESSFUL$
// getcwd fails with EINVAL for a size of 0, and with ERANGE rather than
// write past a buffer the path does not fit, so what it returns is a
// string within the buffer (#358).
#include <assert.h>
#include <string.h>
#include <unistd.h>
int main(void) {
  char buf[8];
  assert(!getcwd(buf, 0));
  char *r = getcwd(buf, sizeof buf);
  if (r)
    assert(r == buf && strlen(r) < sizeof buf);
  return 0;
}
