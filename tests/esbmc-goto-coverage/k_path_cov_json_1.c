int main()
{
  int x = 5, y = 7, z = 9;
  if (x > 3)
  {
    if (y > 5)
    {
      if (z > 7)
      {
        x = x + 1;
      }
    }
  }
  return 0;
}
// Imported from ESBMC regression/goto-coverage/k_path_cov_json_1
// Needs: --k-path-coverage=3 --cov-report-json CHECK_JSON cov-report.json $.coverage_type == "k-path" CHECK_JSON cov-report.json $.summary.total == 8 CHECK_JSON cov-report.json $.summary.covered == 1 CHECK_JSON cov-report.json $.summary.percentage == 12.5 CHECK_JSON cov-report.json $.claims[13].status == "covered"
// RUN: --k-path-coverage=3 --cov-report-json --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^k-Path Coverage: 12\.5%$
// EXPECT: ^Coverage report written to cov-report\.json$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
