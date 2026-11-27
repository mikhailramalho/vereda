/* The last positional argument, and so the only file the pre-fix
 * input_file_check recognised. See first.c. */
int check(void);

int main(void)
{
  return check();
}
// Imported from ESBMC regression/esbmc/github_7434_multi_input_file_yaml_fail
// Needs: --witness-output-yaml
// RUN: first.c --witness-output-yaml - --no-incremental-bmc
// FILES: first.c
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: type: "target"
// EXPECT: type: "function_return"
// EXPECT: file_name: "[^"]*first\.c"
// EXPECT: line: 12$
// EXPECT: (?s)- "[^"]*first\.c".*- "[^"]*main\.c"
// Vereda: verdict none
