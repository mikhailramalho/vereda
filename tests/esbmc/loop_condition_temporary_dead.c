int more();

int main()
{
  int i = 0;
  while (more())
    ++i;
  for (; more(); ++i)
    ;
  return i;
}
// Imported from ESBMC regression/esbmc/loop_condition_temporary_dead
// Needs: --goto-functions-only
// RUN: --goto-functions-only
// MODE: FUTURE
// EXPECT: \$_more\$1 THEN GOTO \d+\n(\s+//[^\n]*\n)?\s+DEAD c:@F@main::\$tmp::return_value\$_more\$1\n
// EXPECT: \$_more\$1 THEN GOTO (\d+)\n[\s\S]*?\n\s+\1: DEAD c:@F@main::\$tmp::return_value\$_more\$1\n
// EXPECT: \$_more\$2 THEN GOTO \d+\n(\s+//[^\n]*\n)?\s+DEAD c:@F@main::\$tmp::return_value\$_more\$2\n
// EXPECT: \$_more\$2 THEN GOTO (\d+)\n[\s\S]*?\n\s+\1: DEAD c:@F@main::\$tmp::return_value\$_more\$2\n
// Vereda: verdict none
