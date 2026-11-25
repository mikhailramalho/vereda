// Models of the printf functions that write to memory: sprintf, snprintf,
// vsprintf and vsnprintf (C17 7.21.6.5, 7.21.6.6, 7.21.6.12, 7.21.6.13),
// glibc's asprintf and vasprintf, and the _chk forms glibc's headers call
// under _FORTIFY_SOURCE. Each byte is a store to the buffer, so a write
// past its end is one the valid dereference checks (issue #29) will see;
// snprintf writes at most n - 1 bytes and the NUL and leaves the rest of
// the buffer alone. The integer, character, string, pointer, %n and %%
// conversions, with their flags, width, precision and length, write the
// bytes glibc writes in the C locale. A floating conversion writes nondet
// bytes, none of them NUL, as many as the field can take: at least one and
// at most its width or the longest text the conversion can give.
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>

void *malloc(size_t);
char __VERIFIER_nondet_char(void);
unsigned __VERIFIER_nondet_uint(void);
void __VERIFIER_assume(int);

// Byte len of the text goes to s[len] if len < cap. These are macros so
// that len stays a value, not memory.
#define PUT(c)                                                                             \
  do {                                                                                     \
    if (len < cap)                                                                         \
      s[len] = (c);                                                                        \
    ++len;                                                                                 \
  } while (0)
#define PAD(c, k)                                                                          \
  for (int k_ = (k); k_ > 0; --k_)                                                         \
  PUT(c)

// The text of %s for the string at p, its wide characters narrowed (the C
// locale's %ls) when wide; returns len past it. A function, called with p
// and with glibc's text for a null p, so that the pointer read is never a
// choice of the two: a read through such a choice takes every known byte,
// the format's too, to the memory arrays, and the format no longer folds.
static size_t string(char *s, size_t cap, size_t len, const void *p, int wide, int prec,
                     int width, int left) {
#define AT(i) (wide ? (char)((const wchar_t *)p)[i] : ((const char *)p)[i])
  int n = 0;
  while ((prec < 0 || n < prec) && AT(n))
    ++n;
  if (!left)
    PAD(' ', width - n);
  for (int i = 0; i < n; ++i)
    PUT(AT(i));
  if (left)
    PAD(' ', width - n);
  return len;
#undef AT
}

// Writes the text of f and ap to s, at most cap bytes of it, and returns its
// whole length.
static size_t format(char *s, size_t cap, const char *f, va_list ap) {
  size_t len = 0;
  for (; *f; ++f) {
    if (*f != '%') {
      PUT(*f);
      continue;
    }
    int left = 0, plus = 0, space = 0, alt = 0, zero = 0, width = 0, prec = -1;
    for (++f;; ++f) {
      if (*f == '-')
        left = 1;
      else if (*f == '+')
        plus = 1;
      else if (*f == ' ')
        space = 1;
      else if (*f == '#')
        alt = 1;
      else if (*f == '0')
        zero = 1;
      else
        break;
    }
    if (*f == '*') {
      width = va_arg(ap, int);
      ++f;
      if (width < 0) {
        left = 1;
        width = -width;
      }
    }
    for (; *f >= '0' && *f <= '9'; ++f)
      width = width * 10 + (*f - '0');
    if (*f == '.') {
      prec = 0;
      if (*++f == '*') {
        prec = va_arg(ap, int);
        ++f;
        if (prec < 0)
          prec = -1;
      }
      for (; *f >= '0' && *f <= '9'; ++f)
        prec = prec * 10 + (*f - '0');
    }
    // The length: H for hh, q for ll and j, l for l, z and t (as wide as
    // long on Linux), h and L as written.
    char size = 0;
    if (*f == 'h' || *f == 'l') {
      size = *f;
      if (f[1] == *f) {
        size = size == 'h' ? 'H' : 'q';
        ++f;
      }
      ++f;
    } else if (*f == 'j' || *f == 'q') {
      size = 'q';
      ++f;
    } else if (*f == 'z' || *f == 't') {
      size = 'l';
      ++f;
    } else if (*f == 'L') {
      size = 'L';
      ++f;
    }
    char conv = *f;
    if (!conv)
      break;
    if (conv == 'C' || conv == 'S') { // glibc's %lc and %ls
      conv += 'a' - 'A';
      size = 'l';
    }
    const void *p = conv == 's' || conv == 'p' ? va_arg(ap, const void *) : NULL;
    if (conv == '%') {
      PUT('%');
    } else if (conv == 'c') {
      char c = va_arg(ap, int); // wint_t for %lc, the same in the C locale
      if (!left)
        PAD(' ', width - 1);
      PUT(c);
      if (left)
        PAD(' ', width - 1);
    } else if (conv == 's' && p) {
      len = string(s, cap, len, p, size == 'l', prec, width, left);
    } else if (conv == 's' || (conv == 'p' && !p)) { // glibc's text for null
      len = string(s, cap, len, conv == 'p' ? "(nil)" : prec < 0 || prec >= 6 ? "(null)" : "",
                   0, conv == 'p' ? -1 : prec, width, left);
    } else if (conv == 'n') {
      void *at = va_arg(ap, void *);
      if (size == 'H')
        *(signed char *)at = len;
      else if (size == 'h')
        *(short *)at = len;
      else if (size == 'l')
        *(long *)at = len;
      else if (size == 'q' || size == 'L')
        *(long long *)at = len;
      else
        *(int *)at = len;
    } else if (conv == 'd' || conv == 'i' || conv == 'u' || conv == 'o' || conv == 'x' ||
               conv == 'X' || conv == 'p') {
      unsigned long long u;
      char sign = 0;
      if (conv == 'p') {
        u = (uintptr_t)p;
        alt = 1;
        conv = 'x';
        size = sizeof p > 4 ? 'q' : 0;
      } else if (conv == 'd' || conv == 'i') {
        long long v = size == 'l'                   ? va_arg(ap, long)
                      : size == 'q' || size == 'L' ? va_arg(ap, long long)
                                                    : va_arg(ap, int);
        if (size == 'H')
          v = (signed char)v;
        else if (size == 'h')
          v = (short)v;
        sign = v < 0 ? '-' : plus ? '+' : space ? ' ' : 0;
        u = v < 0 ? 0ull - v : v;
      } else {
        u = size == 'l'                   ? va_arg(ap, unsigned long)
            : size == 'q' || size == 'L' ? va_arg(ap, unsigned long long)
                                          : va_arg(ap, unsigned);
        if (size == 'H')
          u = (unsigned char)u;
        else if (size == 'h')
          u = (unsigned short)u;
      }
      unsigned base = conv == 'o' ? 8 : conv == 'x' || conv == 'X' ? 16 : 10;
      char ten = conv == 'X' ? 'A' : 'a';
      // The digits, last first, from the end of tmp: each iteration writes
      // a fixed place. A value of 32 bits or fewer divides as 32 bits,
      // which the solver finds cheaper.
      char tmp[24];
      int nd = 0;
#define DIGITS(T)                                                                          \
  for (T v = u;; v /= base) {                                                              \
    unsigned d = v % base;                                                                 \
    tmp[sizeof tmp - ++nd] = d < 10 ? '0' + d : ten + d - 10;                              \
    if (v < base)                                                                          \
      break;                                                                               \
  }
      if (u || prec) {
        if (size == 'q' || size == 'L' || (size == 'l' && sizeof(long) > 4))
          DIGITS(unsigned long long)
        else
          DIGITS(unsigned)
      }
      int zeros = prec > nd ? prec - nd : 0;
      if (conv == 'o' && alt && !zeros && (u || !nd))
        zeros = 1;
      int prefix = alt && u && (conv == 'x' || conv == 'X');
      int fill = width - (sign != 0) - 2 * prefix - zeros - nd;
      if (zero && !left && prec < 0 && fill > 0) {
        zeros += fill;
        fill = 0;
      }
      if (!left)
        PAD(' ', fill);
      if (sign)
        PUT(sign);
      if (prefix) {
        PUT('0');
        PUT(conv);
      }
      PAD('0', zeros);
      for (int i = sizeof tmp - nd; i < (int)sizeof tmp; ++i)
        PUT(tmp[i]);
      if (left)
        PAD(' ', fill);
    } else if (conv == 'f' || conv == 'F' || conv == 'e' || conv == 'E' || conv == 'g' ||
               conv == 'G' || conv == 'a' || conv == 'A') {
      if (size == 'L')
        (void)va_arg(ap, long double);
      else
        (void)va_arg(ap, double);
      // The longest text: sign, digits, point, exponent. %f has as many
      // integer digits as the largest value, 309 for double and 4933 for
      // long double; the exponents have up to 4 digits; %a's mantissa has
      // 13 hex digits after the point for double, 16 for long double.
      unsigned digits = prec < 0 ? 6 : prec;
      unsigned most = conv == 'f' || conv == 'F' ? (size == 'L' ? 4933 : 309) + digits + 2
                      : conv == 'e' || conv == 'E' ? digits + 9
                      : conv == 'g' || conv == 'G' ? (digits ? digits : 1) + 8
                      : (prec < 0 ? 16 : digits) + 12;
      if (most < (unsigned)width)
        most = width;
      unsigned lo = (unsigned)width > 1 ? (unsigned)width : 1;
      unsigned k = __VERIFIER_nondet_uint();
      if (k < lo)
        k = lo;
      if (k > most)
        k = lo;
      if (len < cap) {
        size_t avail = cap - len;
        for (unsigned i = 0; i < most; ++i) {
          if ((i < k) & (i < avail)) {
            char c = __VERIFIER_nondet_char();
            if (!c)
              c = 1;
            s[len + i] = c;
          }
        }
      }
      len += k;
    }
  }
  return len;
}

// The result as an int; a length past INT_MAX fails (EOVERFLOW in POSIX).
static int result(size_t len) { return len > INT_MAX ? -1 : (int)len; }

int vsnprintf(char *s, size_t n, const char *f, va_list ap) {
  size_t len = format(s, n ? n - 1 : 0, f, ap);
  if (n)
    s[len < n - 1 ? len : n - 1] = 0;
  return result(len);
}

int vsprintf(char *s, const char *f, va_list ap) { return vsnprintf(s, SIZE_MAX, f, ap); }

// Two passes, the first to size the object: the second must give the same
// length, which keeps the nondet bytes of a floating conversion apart from
// their count.
int vasprintf(char **p, const char *f, va_list ap) {
  va_list aq;
  va_copy(aq, ap);
  size_t len = format(NULL, 0, f, aq);
  va_end(aq);
  char *s = malloc(len + 1);
  if (!s)
    return -1;
  __VERIFIER_assume(format(s, len, f, ap) == len);
  s[len] = 0;
  *p = s;
  return result(len);
}

int sprintf(char *s, const char *f, ...) {
  va_list ap;
  va_start(ap, f);
  int r = vsprintf(s, f, ap);
  va_end(ap);
  return r;
}

int snprintf(char *s, size_t n, const char *f, ...) {
  va_list ap;
  va_start(ap, f);
  int r = vsnprintf(s, n, f, ap);
  va_end(ap);
  return r;
}

int asprintf(char **p, const char *f, ...) {
  va_list ap;
  va_start(ap, f);
  int r = vasprintf(p, f, ap);
  va_end(ap);
  return r;
}

// glibc's checked forms abort when the text would not fit in slen, the size
// the compiler sees for s; here the write itself is checked against the
// object instead, so they ignore flag and slen.
int __vsprintf_chk(char *s, int flag, size_t slen, const char *f, va_list ap) {
  return vsprintf(s, f, ap);
}

int __vsnprintf_chk(char *s, size_t n, int flag, size_t slen, const char *f, va_list ap) {
  return vsnprintf(s, n, f, ap);
}

int __vasprintf_chk(char **p, int flag, const char *f, va_list ap) {
  return vasprintf(p, f, ap);
}

int __sprintf_chk(char *s, int flag, size_t slen, const char *f, ...) {
  va_list ap;
  va_start(ap, f);
  int r = vsprintf(s, f, ap);
  va_end(ap);
  return r;
}

int __snprintf_chk(char *s, size_t n, int flag, size_t slen, const char *f, ...) {
  va_list ap;
  va_start(ap, f);
  int r = vsnprintf(s, n, f, ap);
  va_end(ap);
  return r;
}

int __asprintf_chk(char **p, int flag, const char *f, ...) {
  va_list ap;
  va_start(ap, f);
  int r = vasprintf(p, f, ap);
  va_end(ap);
  return r;
}
