#include <stdio.h>
#include <string.h>

#define _4GB 10000
#define _2GB 5000
#define _32MB 1000
#define _16MB 500
#define _2MB 100

typedef struct _RANGE
{
  unsigned int len;
  unsigned int base;
  unsigned int top;
} RANGE;

unsigned int __VERIFIER_nondet_ull()
{
  unsigned int val;
  return val;
}

typedef union N_RANGE
{
  RANGE Raw[4];
  struct
  {
    RANGE SegArr[2];
    RANGE ObjArr[2];
  };
} N_RANGE;

N_RANGE RangeArr;

void __VERIFIER_nondet_RANGE_by_fields(RANGE *val)
{
  val->len = __VERIFIER_nondet_ull();
  val->base = __VERIFIER_nondet_ull();
  val->top = val->len + val->base;
}

RANGE __VERIFIER_nondet_RANGE()
{
  RANGE val;

  val.len = __VERIFIER_nondet_ull();
  val.base = __VERIFIER_nondet_ull();
  val.top = val.len + val.base;

  return val;
}

#define __VERIFIER_nondet_RANGE_fields(some_range)                                        \
  ({                                                                           \
    (some_range).base = __VERIFIER_nondet_ull();                                          \
    (some_range).len = __VERIFIER_nondet_ull();                                           \
    (some_range).top = (some_range).base + (some_range).len;                   \
  })

void range_fun()
{
  unsigned int i = 0;
  unsigned int sum_rng_len = 0;
  unsigned int highest_range_top = _4GB - _16MB;
  unsigned int current_base;
  unsigned int current_len;

  for(; i < 4; i++)
  {
    RangeArr.Raw[i] = __VERIFIER_nondet_RANGE();
    __VERIFIER_nondet_RANGE_fields(RangeArr.Raw[i]);
    __VERIFIER_assume(RangeArr.Raw[i].base >= _4GB - _16MB);

    current_base = RangeArr.Raw[i].base;
    current_len = RangeArr.Raw[i].len;
    sum_rng_len += RangeArr.Raw[i].len;
    if(i < 2)
      if(RangeArr.Raw[i].top > highest_range_top)
        highest_range_top = RangeArr.Raw[i].top;

  }

  __VERIFIER_assert(RangeArr.Raw[0].base == _2GB); // RangeArr base value 1
}

int main()
{
  range_fun();
  return 0;
}
// Imported from ESBMC regression/nonz3/github_371_6
// RUN: --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
