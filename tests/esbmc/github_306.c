#define CODIV 0.003921568627451
#define SIZE 25

unsigned char __VERIFIER_nondet_uchar();

int main() {
  unsigned short i;
  float image[SIZE];

  for (i = 0; i < SIZE; i++) 
    image[i] = __VERIFIER_nondet_uchar();

  for (i = 0; i < SIZE; i++)
    image[i] = (image[i] * CODIV);

  for (i = 0; i < SIZE; i++)
    assert(image[i] >= 0);

  return 0;
}
// Imported from ESBMC regression/esbmc/github_306
// RUN: --no-incremental-bmc
// MODE: THOROUGH
// EXPECT: ^VERIFICATION SUCCESSFUL$
