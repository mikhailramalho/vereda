int a[2];
int i,j;

p(){
  int tmp;
  tmp=a[i];
  a[i]=a[j];
  a[j]=tmp;
}

int main(){
  i=0;
  j=1;
  a[i]=100;
  a[j]=200;
  p();
  ;
  p();
  ;
  p();
  ;
  if(a[i]==100){
    assert(0);    
  } else {
    ;
  }
}

// Imported from ESBMC regression/esbmc/02_eureka16
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
