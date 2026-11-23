#include <stdlib.h>

struct s1 {
  int type;
};

struct s2 {
  struct s1;
  char ch;
};

struct s2 *make_s2(char ch)
{
  struct s2 *s = malloc (sizeof *s);
  s->type = 0;
  s->ch = ch;
  return s;
}
// Imported from ESBMC regression/esbmc/github_170
// Needs: -fms-extensions
// RUN: -fms-extensions -Wno-microsoft-anon-tag --function make_s2 --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
