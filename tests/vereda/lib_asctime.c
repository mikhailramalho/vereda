// asctime, asctime_r, ctime and ctime_r format broken-down and epoch times
// into a 25-character string ending in '\n' (C17 7.27.3.1, 7.27.3.2).
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
#include <time.h>
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);

int main(void) {
  struct tm t = {0};
  t.tm_year = 100;
  t.tm_mon = 0;
  t.tm_mday = 1;
  t.tm_wday = 6;
  char *s = asctime(&t);
  __VERIFIER_assert(s != NULL);
  __VERIFIER_assert(strlen(s) == 25);
  __VERIFIER_assert(strcmp(s, "Sat Jan  1 00:00:00 2000\n") == 0);

  struct tm t2 = {0};
  t2.tm_year = 124;
  t2.tm_mon = 11;
  t2.tm_mday = 31;
  t2.tm_hour = 23;
  t2.tm_min = 59;
  t2.tm_sec = 59;
  t2.tm_wday = 2;
  char buf[26];
  char *r = asctime_r(&t2, buf);
  __VERIFIER_assert(r == buf);
  __VERIFIER_assert(strcmp(buf, "Tue Dec 31 23:59:59 2024\n") == 0);

  time_t epoch = 0;
  char *c = ctime(&epoch);
  __VERIFIER_assert(c == s);
  __VERIFIER_assert(strcmp(c, "Thu Jan  1 00:00:00 1970\n") == 0);

  time_t before = -86400;
  __VERIFIER_assert(ctime_r(&before, buf) == buf);
  __VERIFIER_assert(strcmp(buf, "Wed Dec 31 00:00:00 1969\n") == 0);

  time_t recent = 1705322400;
  __VERIFIER_assert(ctime_r(&recent, buf) == buf);
  __VERIFIER_assert(strcmp(buf, "Mon Jan 15 12:40:00 2024\n") == 0);

  struct tm sym = {0};
  sym.tm_wday = __VERIFIER_nondet_int();
  sym.tm_mon = __VERIFIER_nondet_int();
  sym.tm_mday = __VERIFIER_nondet_int();
  sym.tm_hour = __VERIFIER_nondet_int();
  sym.tm_min = __VERIFIER_nondet_int();
  sym.tm_sec = __VERIFIER_nondet_int();
  sym.tm_year = __VERIFIER_nondet_int();
  __VERIFIER_assert(asctime_r(&sym, buf) == buf);
  __VERIFIER_assert(buf[3] == ' ' && buf[7] == ' ' && buf[10] == ' ');
  __VERIFIER_assert(buf[13] == ':' && buf[16] == ':' && buf[19] == ' ');
  __VERIFIER_assert(buf[24] == '\n' && buf[25] == '\0');
  return 0;
}
