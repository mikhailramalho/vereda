struct dev {
  char pad[432];
  void *priv;
};

struct dev d;
_Bool __VERIFIER_nondet_bool(void);

void *detach(struct dev *dev) {
  unsigned long a = (unsigned long)dev;
  unsigned long b = a + 432;
  return *((void **)b);
}

int main() {
  struct dev *p = __VERIFIER_nondet_bool() ? &d : 0;
  if (p)
    return detach(p) != 0;
  return 0;
}
// Imported from ESBMC regression/esbmc/null_offset_int_read
// EXPECT: ^VERIFICATION SUCCESSFUL$
