// Uninitialised bytes in a byval struct argument stay uninitialised in the copy.
// RUN: -m64 --ub-uninit-check
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: read of uninitialised memory
struct Big {
  long a;
  long b;
  long c;
  long d;
};

static long read_b(struct Big s) {
  return s.b;
}

int main(void) {
  struct Big orig;
  orig.a = 1;
  orig.c = 3;
  orig.d = 4;
  return (int)read_b(orig);
}
