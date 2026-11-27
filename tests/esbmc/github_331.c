struct _twoIntsStruct {
  int intOne;
  int intTwo;
};

typedef struct _twoIntsStruct twoIntsStruct;

int main() {
  twoIntsStruct *dataBuffer = malloc(16UL);
  (dataBuffer + 1)->intOne = 1;
}
// Imported from ESBMC regression/esbmc/github_331
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION SUCCESSFUL$
