int a;
main()
{
  pthread_mutex_init();
  printf(a);
}
// Imported from ESBMC regression/parallel-solving/pthread_mutex_init
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
