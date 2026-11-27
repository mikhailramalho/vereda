// An endless loop with an overflow check leaves through the check's trap,
// so it is unrolled, which finds the overflow in the second iteration.
// RUN: --ub-signed-overflow-check
// EXPECT: ^VERIFICATION FAILED$
#include <limits.h>

int main(void) {
  int x = INT_MAX - 1;
  for (;;)
    x++;
}
