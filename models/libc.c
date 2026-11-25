// Models of glibc internals and small <time.h> and <errno.h> functions.
//
// glibc's <ctype.h> macros index the tables that __ctype_b_loc,
// __ctype_tolower_loc and __ctype_toupper_loc point to with any value from
// -128 to 255 (EOF included). Only the C locale is modelled: bytes outside
// ASCII belong to no class and map to themselves. glibc keeps each class
// bit byte-swapped on little-endian targets (_ISbit in <ctype.h>).
#include <stdint.h>
#include <time.h>

#define BIT(b) ((b) < 8 ? (1 << (b)) << 8 : (1 << (b)) >> 8)
#define UPPER(c) ((c) >= 'A' && (c) <= 'Z')
#define LOWER(c) ((c) >= 'a' && (c) <= 'z')
#define DIGIT(c) ((c) >= '0' && (c) <= '9')
#define ALPHA(c) (UPPER(c) || LOWER(c))
#define XDIGIT(c) (DIGIT(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#define SPACE(c) ((c) == ' ' || ((c) >= '\t' && (c) <= '\r'))
#define PRINT(c) ((c) >= ' ' && (c) <= '~')
#define GRAPH(c) ((c) > ' ' && (c) <= '~')
#define BLANK(c) ((c) == ' ' || (c) == '\t')
#define CNTRL(c) (((c) >= 0 && (c) < ' ') || (c) == 127)
#define PUNCT(c) (GRAPH(c) && !ALPHA(c) && !DIGIT(c))
#define CLASSES(c)                                                                         \
  ((UPPER(c) ? BIT(0) : 0) | (LOWER(c) ? BIT(1) : 0) | (ALPHA(c) ? BIT(2) : 0) |         \
   (DIGIT(c) ? BIT(3) : 0) | (XDIGIT(c) ? BIT(4) : 0) | (SPACE(c) ? BIT(5) : 0) |        \
   (PRINT(c) ? BIT(6) : 0) | (GRAPH(c) ? BIT(7) : 0) | (BLANK(c) ? BIT(8) : 0) |         \
   (CNTRL(c) ? BIT(9) : 0) | (PUNCT(c) ? BIT(10) : 0) | (ALPHA(c) || DIGIT(c) ? BIT(11) : 0))
// One entry per value from -128 to 255, built at compile time.
#define R4(f, c) f(c), f((c) + 1), f((c) + 2), f((c) + 3)
#define R16(f, c) R4(f, c), R4(f, (c) + 4), R4(f, (c) + 8), R4(f, (c) + 12)
#define R64(f, c) R16(f, c), R16(f, (c) + 16), R16(f, (c) + 32), R16(f, (c) + 48)
#define TABLE(f) {R64(f, -128), R64(f, -64), R64(f, 0), R64(f, 64), R64(f, 128), R64(f, 192)}
#define CLASS(c) ((c) >= 0 && (c) < 128 ? CLASSES(c) : 0)
#define TOLOWER(c) (UPPER(c) ? (c) + 32 : (c))
#define TOUPPER(c) (LOWER(c) ? (c) - 32 : (c))

static const unsigned short ctype_b[384] = TABLE(CLASS);
static const int32_t ctype_tolower[384] = TABLE(TOLOWER);
static const int32_t ctype_toupper[384] = TABLE(TOUPPER);

const unsigned short **__ctype_b_loc(void) {
  static const unsigned short *table = ctype_b + 128;
  return &table;
}

const int32_t **__ctype_tolower_loc(void) {
  static const int32_t *table = ctype_tolower + 128;
  return &table;
}

const int32_t **__ctype_toupper_loc(void) {
  static const int32_t *table = ctype_toupper + 128;
  return &table;
}

int tolower(int c) { return c >= -128 && c < 256 ? ctype_tolower[c + 128] : c; }
int toupper(int c) { return c >= -128 && c < 256 ? ctype_toupper[c + 128] : c; }

// The classification functions, for calls that bypass the macros: the
// class formulas themselves, cheaper for the solver than a table read.
int isalnum(int c) { return ALPHA(c) || DIGIT(c); }
int isalpha(int c) { return ALPHA(c); }
int isblank(int c) { return BLANK(c); }
int iscntrl(int c) { return CNTRL(c); }
int isdigit(int c) { return DIGIT(c); }
int isgraph(int c) { return GRAPH(c); }
int islower(int c) { return LOWER(c); }
int isprint(int c) { return PRINT(c); }
int ispunct(int c) { return PUNCT(c); }
int isspace(int c) { return SPACE(c); }
int isupper(int c) { return UPPER(c); }
int isxdigit(int c) { return XDIGIT(c); }

// errno, zero at program start (C17 7.5p3). The walker finds it by name: a
// call without a body may set it.
int __vereda_errno;
int *__errno_location(void) { return &__vereda_errno; }

long __VERIFIER_nondet_long(void);

// Any time: the clock is outside the program.
time_t time(time_t *t) {
  time_t now = __VERIFIER_nondet_long();
  if (t)
    *t = now;
  return now;
}

static const char wday_name[7][3] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char mon_name[12][3] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                     "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static char asctime_buf[26];

// C17 7.27.3.1 and POSIX asctime_r: formats tm into the 26-byte buffer as
// "Www Mmm dd hh:mm:ss yyyy\n\0". Out-of-range fields are wrapped so the
// model writes 25 non-NUL bytes and a NUL without an internal bounds failure.
char *asctime_r(const struct tm *tm, char *buf) {
  unsigned wday = (unsigned)tm->tm_wday < 7 ? (unsigned)tm->tm_wday : 0;
  unsigned mon = (unsigned)tm->tm_mon < 12 ? (unsigned)tm->tm_mon : 0;
  unsigned mday = (unsigned)tm->tm_mday % 100;
  unsigned hour = (unsigned)tm->tm_hour % 100;
  unsigned min = (unsigned)tm->tm_min % 100;
  unsigned sec = (unsigned)tm->tm_sec % 100;
  unsigned year = (unsigned)(1900 + tm->tm_year) % 10000;

  buf[0] = wday_name[wday][0];
  buf[1] = wday_name[wday][1];
  buf[2] = wday_name[wday][2];
  buf[3] = ' ';
  buf[4] = mon_name[mon][0];
  buf[5] = mon_name[mon][1];
  buf[6] = mon_name[mon][2];
  buf[7] = ' ';
  buf[8] = mday >= 10 ? (char)('0' + mday / 10) : ' ';
  buf[9] = (char)('0' + mday % 10);
  buf[10] = ' ';
  buf[11] = (char)('0' + hour / 10);
  buf[12] = (char)('0' + hour % 10);
  buf[13] = ':';
  buf[14] = (char)('0' + min / 10);
  buf[15] = (char)('0' + min % 10);
  buf[16] = ':';
  buf[17] = (char)('0' + sec / 10);
  buf[18] = (char)('0' + sec % 10);
  buf[19] = ' ';
  buf[20] = (char)('0' + year / 1000);
  buf[21] = (char)('0' + (year / 100) % 10);
  buf[22] = (char)('0' + (year / 10) % 10);
  buf[23] = (char)('0' + year % 10);
  buf[24] = '\n';
  buf[25] = '\0';
  return buf;
}

char *asctime(const struct tm *tm) { return asctime_r(tm, asctime_buf); }

// C17 7.27.3.2 and POSIX ctime_r: formats *timer in UTC (the C locale's
// timezone), breaking down the epoch timestamp with Hinnant's civil-from-days
// formula so there are no loops to unroll.
char *ctime_r(const time_t *timer, char *buf) {
  long long t = *timer;
  long long days = t / 86400;
  long long rem = t % 86400;
  if (rem < 0) {
    rem += 86400;
    --days;
  }
  struct tm tm;
  tm.tm_sec = (int)(rem % 60);
  rem /= 60;
  tm.tm_min = (int)(rem % 60);
  tm.tm_hour = (int)(rem / 60);
  tm.tm_wday = (int)(((days + 4) % 7 + 7) % 7);

  long long z = days + 719468;
  long long era = (z >= 0 ? z : z - 146096) / 146097;
  unsigned long long doe = (unsigned long long)(z - era * 146097);
  unsigned long long yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  long long y = (long long)yoe + era * 400;
  unsigned long long doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  unsigned long long mp = (5 * doy + 2) / 153;
  tm.tm_mday = (int)(doy - (153 * mp + 2) / 5 + 1);
  tm.tm_mon = (int)(mp < 10 ? mp + 2 : mp - 10);
  tm.tm_year = (int)(y + (tm.tm_mon <= 1) - 1900);
  return asctime_r(&tm, buf);
}

char *ctime(const time_t *timer) { return ctime_r(timer, asctime_buf); }
