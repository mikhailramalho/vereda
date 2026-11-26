int a;
main()
{
  pthread_mutex_init();
  printf(a);
}
// Imported from ESBMC regression/parallel-solving/pthread_mutex_init
// RUN: -Wno-error=implicit-int -Wno-error=int-conversion -Wno-error=format-security
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
