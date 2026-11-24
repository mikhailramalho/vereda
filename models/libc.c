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
