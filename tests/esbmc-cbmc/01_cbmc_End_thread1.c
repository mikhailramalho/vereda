void pthread_exit(void *value_ptr);

int main()
{
  int i;

  if(i==100)
    pthread_exit(0);

  assert(i!=100);
}
// Imported from ESBMC regression/cbmc/01_cbmc_End_thread1
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
