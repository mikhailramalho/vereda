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
// RUN: -m32 --big-endian --interval-analysis
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
