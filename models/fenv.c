// Models of the rounding-mode functions of <fenv.h> (C17 7.6.3). The mode
// is __vereda_rounding, which the walker reads for each floating-point
// operation clang emits with dynamic rounding (under -frounding-math, which
// vereda.py adds when a source calls fesetround); every other operation
// rounds to nearest.
#include <fenv.h>

int __vereda_rounding = FE_TONEAREST;

int fegetround(void) { return __vereda_rounding; }

// As glibc on x86: a value with bits outside the rounding-control field is
// no mode, and fails with the mode unchanged.
int fesetround(int round) {
  if (round & ~(FE_TONEAREST | FE_DOWNWARD | FE_UPWARD | FE_TOWARDZERO))
    return 1;
  __vereda_rounding = round;
  return 0;
}
