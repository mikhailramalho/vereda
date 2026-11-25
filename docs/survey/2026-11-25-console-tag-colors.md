# Console tag colors

Use bold blue for `[INFO]`, cyan for `[PROGRESS]`, and bold magenta for
`[RESULTS]`. The results heading uses the same helper for successful,
failed and unknown runs; individual verdicts keep their existing colors.
Warnings remain bold yellow. LLVM's existing automatic color handling
keeps redirected output plain, with the same text and spacing as before.

Checked terminal escape sequences and plain redirected output for successful,
failed and unknown runs. Six focused output, banner and verdict regressions pass.
