#include <assert.h>

typedef struct
{
  int a;
} b;
typedef struct
{
  b c[100];
  int d;
} e;
typedef struct
{
  e c;
} f;
b g = {0 == 0};
f h;
void i(f *j)
{
  for (int i = 0; i < 2; ++i)
    j->c.c[j->c.d] = g;

  assert(j->c.c[j->c.d].a != 1);
}
int main()
{
  i(&h);
}
// Imported from ESBMC regression/esbmc/rel_op_typecast_fail
// EXPECT: ^VERIFICATION FAILED$
