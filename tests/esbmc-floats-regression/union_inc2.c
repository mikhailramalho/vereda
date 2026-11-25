  union {
    float a;
    int b;
  } from_union = { .a = 1.0f };

int main(void)
{
  from_union.b++;
  float a = from_union.a;
  assert(from_union.a == 0x1.000002p+0);
}

// Imported from ESBMC regression/floats-regression/union_inc2
// Needs: --big-endian --interval-analysis
// RUN: --32 --big-endian --interval-analysis -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
