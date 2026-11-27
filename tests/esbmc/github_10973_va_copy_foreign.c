// q is copied from the caller's va_list, so vprintf formats the caller's
// arguments, not g's own `bad`.
#include <stdarg.h>
#include <stdio.h>

static void g(va_list *outer, const char *s, ...)
{
  va_list q;
  va_copy(q, *outer);
  vprintf(s, q);
  va_end(q);
}

static void f(const char *s, ...)
{
  va_list p;
  char bad[1] = {'x'};
  va_start(p, s);
  g(&p, s, bad);
  va_end(p);
}

int main(void)
{
  f("%s", "ok");
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10973_va_copy_foreign
// Needs: --printf-check
// RUN: --printf-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
