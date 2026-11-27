int main()
{
  int x = 5;
  if (x > 3)
  {
    x = x + 1;
  }
  return 0;
}
// Imported from ESBMC regression/goto-coverage/cov_report_json_1
// Needs: --branch-coverage --cov-report-json CHECK_JSON cov-report.json $.coverage_type == "branch" CHECK_JSON cov-report.json $.summary.total == 2 CHECK_JSON cov-report.json $.summary.covered == 1 CHECK_JSON cov-report.json $.summary.percentage == 50.0
// RUN: --branch-coverage --cov-report-json --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Coverage report written to cov-report\.json$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
