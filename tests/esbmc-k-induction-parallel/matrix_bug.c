//  Aluno: Fredson Souza da Costa  Matrícula: 20902628
//  Verificação de propriedades de segurança
//  programa: I) ENCONTRA O MAIOR ELEMENTO EM UMA MATRIZ

#ifdef ESBMC
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#endif

#define NULL 0

int main()
{
  unsigned int N_LIN;
  unsigned int N_COL;
      
  unsigned int j,k;
  int matriz[N_COL][N_LIN], maior;
  
  maior = __VERIFIER_nondet_int();
  for(j=0;j<N_COL;j++)
    for(k=0;k<N_LIN;k++)
    {       
       matriz[j][k] = __VERIFIER_nondet_int();
       
       if(matriz[j][k]>maior)
          maior = matriz[j][k];                          
    }                       
    
  for(j=0;j<N_COL;j++)
    for(k=0;k<N_LIN;k++)
      assert(matriz[j][k]<maior);    
}

// Imported from ESBMC regression/k-induction-parallel/matrix_bug
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
