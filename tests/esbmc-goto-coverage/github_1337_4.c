   #include <assert.h>

   void loop1()
   {
      for (int i = 0; i < 2; i++)
      {
         assert(0);
      }
   }

   void loop2()
   {
      for (int i = 0; i < 6; i++)
      {
         assert(1);
      }
   }

   int main()

   {
      switch (__VERIFIER_nondet_int())
      {
      case 1:
         loop1();
         break;
      case 2:
         loop2();
         break;
      default:
         ;
      }
      return 0;
   }
// Imported from ESBMC regression/goto-coverage/github_1337_4
// Needs: --assertion-coverage
// RUN: --assertion-coverage --no-incremental-bmc
// EXPECT: ^Total Asserts: 8
// EXPECT: ^Total Assertion Instances: 8
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// MODE: FUTURE
// Vereda: verdict none
