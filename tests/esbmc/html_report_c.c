int __VERIFIER_nondet_int();

int main()
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0 && x < 10);
  int y = x + 1;
  __VERIFIER_assert(y > 100); // y must exceed one hundred
  int from = 0;
  return 0;
}
// Imported from ESBMC regression/esbmc/html_report_c
// Needs: --generate-html-report CHECK_FILE report-1.html contains <!doctype html>\s*<html>\s*<head> CHECK_FILE report-1.html contains <title>ESBMC report</title> CHECK_FILE report-1.html contains <h3>Bug Summary</h3>\s*<table class=["']simpletable["']> CHECK_FILE report-1.html contains <td class=["']rowname["']>File:</td>\s*<td>[^<]*html_report_c[\\/]html_report_c\.c</td> CHECK_FILE report-1.html contains <a href=["']#EndPath["']>function main, line 8, column 3</a>\s*<br />Y must exceed one hundred CHECK_FILE report-1.html contains esbmc [^<]*--generate-html-report=1 CHECK_FILE report-1.html contains <div id=["']?File1["']?>\s*<h4 class=["']?FileName["']?>[^<]*html_report_c[\\/]html_report_c\.c</h4> CHECK_FILE report-1.html contains <table class=["']code["'] data-fileid=["']1["']> CHECK_FILE report-1.html contains data-linenumber=["']1["']>\s*<td class=["']num["'] id=["']LN1["']>1</td>\s*<td class=["']line["']><span class=["']keyword["']>int</span> __VERIFIER_nondet_int\(\); CHECK_FILE report-1.html contains <span class=["']keyword["']>int</span> CHECK_FILE report-1.html absent <span class=["']keyword["']>from</span> CHECK_FILE report-1.html contains <td>y = [0-9]+</td> CHECK_FILE report-1.html contains Assumption restriction CHECK_FILE report-1.html contains class=["']PathIndex PathIndexControl["']>[0-9]+</div> CHECK_FILE report-1.html contains <div id=["']Path[0-9]+["'] class=["']msg msgEvent["'] CHECK_FILE report-1.html contains <div id=["']EndPath["'] class=["']msg msgEvent["'] CHECK_FILE report-1.html contains var relevant_lines = \{"1": \{ CHECK_FILE report-1.html contains "8": 1 CHECK_FILE report-1.html contains </table>\s*<hr class=["']?divider["']?> CHECK_FILE report-1.html contains </body>\s*</html>
// RUN: --generate-html-report --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Generating HTML report for trace: 1$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
