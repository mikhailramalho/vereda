// EXPECT: ^VERIFICATION FAILED$
// readv writes the buffers its iovec array points to (#114).
#include <sys/uio.h>
extern void __VERIFIER_assert(int);
int main(void) {
  char b[4] = {0};
  struct iovec v = {b, sizeof b};
  readv(0, &v, 1);
  __VERIFIER_assert(b[0] == 0);
  return 0;
}
