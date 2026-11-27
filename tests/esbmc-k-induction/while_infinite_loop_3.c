
int x=0;

void eval(void) 
{
  while (1) {
      x=1;
      break;
  }
  return;
}


int main() {

  while(1)
  {
    eval();
    assert(x==0);    
  }

  assert(x!=0);

  return 0;
}

// Imported from ESBMC regression/k-induction/while_infinite_loop_3
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
