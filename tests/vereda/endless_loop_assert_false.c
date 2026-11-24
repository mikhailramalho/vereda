// An endless loop with an assertion is unrolled, which finds it failing in
// the second iteration.
// EXPECT: ^VERIFICATION FAILED$
#include <assert.h>

int main(void) {
  for (int i = 0;; i++)
    assert(i < 1);
}
