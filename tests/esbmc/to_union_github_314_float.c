#include <math.h>

typedef union{
    double raw64;
    struct {
        unsigned raw32lo;
        unsigned raw32hi;
    };
    struct {
        unsigned long long    field1:1;
        unsigned long long    field2:1;
    };
} union_t;


double foo(double addr) {
    return addr + 100;
}

int main() {
  double nondet = __VERIFIER_nondet_double();
  __VERIFIER_assume(!isnan(nondet));
   union_t x = (union_t) foo(nondet);
   __VERIFIER_assert(x.raw64 == (nondet+100)); // double value should initialize union_t properly
   return 0;
}
// Imported from ESBMC regression/esbmc/to_union_github_314_float
// EXPECT: ^VERIFICATION SUCCESSFUL$
