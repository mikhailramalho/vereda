/* The IREP2 pass changes nothing in this body: migrate_expr already decayed
   the array in the conditional arm, and the call's argument needs no
   conversion the converter did not emit. adjust() therefore leaves the legacy
   value alone, and --symbol-table-only prints the converter's tree unless the
   write-back is forced. */
char b[4];
char *d;
void snk(void *p);

int main(int argc, char **argv)
{
  char *c;
  c = argc == 1 ? b : d;
  snk(c);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_writeback_all
// Needs: --symbol-table-only --clang-c-irep2-adjust-only --clang-c-irep2-adjust-writeback-all
// RUN: --symbol-table-only --clang-c-irep2-adjust-only --clang-c-irep2-adjust-writeback-all
// MODE: FUTURE
// EXPECT: c = argc == 1 \? &b\[0\] : d;
// Vereda: verdict none
