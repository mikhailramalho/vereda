// RUN: --no-summarize-array-loops --unwind 4
// FILES: helper.ll
// EXPECT: Unwinding loop 0 iteration 1   file .*main.c line [0-9]+.* function main
// EXPECT: ^\[PROGRESS\] Unwinding loop 0 iteration 1   function model_loop\(\)$
// EXPECT-NOT: file <unknown> line 0
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern int model_loop(void) __asm__("_Z10model_loopv");
int main(void) {
  for (int i = 0; i < 2; ++i)
    model_loop();
}
