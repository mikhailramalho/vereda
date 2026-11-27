struct dev {
  char pad[432];
  void *priv;
};

struct dev *var_group1;

/* CIL lowers dev->priv to integer arithmetic on the pointer; with dev NULL the
 * read is at address 432, which int_to_ptr does not place in NULL's object. */
void *detach(struct dev *dev) {
  unsigned long a = (unsigned long)dev;
  unsigned long b = a + 432;
  return *((void **)b);
}

int main() { return detach(var_group1) != 0; }
// Imported from ESBMC regression/esbmc/null_offset_int_read_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  dereference failure: NULL pointer$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
