// Models of <math.h> classification functions, for calls that bypass the
// macros (a call without <math.h> passes a double). C17 7.12.3.
int isnan(double x) { return x != x; }
int isinf(double x) { return x == __builtin_inf() || x == -__builtin_inf(); }
int isfinite(double x) { return !isnan(x) && !isinf(x); }
int signbit(double x) { return __builtin_signbit(x); }

// atan as ESBMC's model computes it (Cody and Waite's rational
// approximation, C17 7.12.4.3); a zero and a NaN come back as they are
// and an infinity gives pi/2 with its sign (F.10.1.3). atanf and atanl
// compute it in double.
static const double atanBase[] = {0, 3.14159265358979323846 / 6, 3.14159265358979323846 / 2,
                                  3.14159265358979323846 / 3};
double atan(double x) {
  if (x == 0)
    return x;
  double f = __builtin_fabs(x);
  int n = 0;
  if (f > 1) {
    f = 1 / f;
    n = 2;
  }
  if (f > 0.26794919243112270) {
    f = (((0.73205080756887729 * f - 0.5) - 0.5) + f) / (1.7320508075688772 + f);
    n++;
  }
  if (__builtin_fabs(f) > 0x1p-28) {
    double g = f * f;
    double q = (((g + 0.150240011600285761e2) * g + 0.595784361425973445e2) * g +
                0.861573495971302425e2) * g + 0.410663066825757813e2;
    double r = (((-0.837582993681500593e0 * g - 0.849462403513206835e1) * g -
                 0.205058551958616520e2) * g - 0.136887688941919269e2) * g / q;
    f += f * r;
  }
  if (n > 1)
    f = -f;
  f += atanBase[n];
  return x < 0 ? -f : f;
}
float atanf(float x) { return atan(x); }
long double atanl(long double x) { return atan(x); }
