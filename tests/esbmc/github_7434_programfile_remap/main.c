#include "helper.h"

int main(void)
{
  int d = 0;
  return helper_div(10, d);
}
// Imported from ESBMC regression/esbmc/github_7434_programfile_remap
// Needs: --witness-output-graphml - --witness-programfile
// RUN: --witness-output-graphml - --witness-programfile programfile.c
// FILES: programfile.c
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: key="originfile">[^<]*helper\.h
// EXPECT: <data key="startline">6</data>
// Vereda: verdict none
