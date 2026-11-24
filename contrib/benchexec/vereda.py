"""BenchExec tool-info module for Vereda: copy it to benchexec/tools/vereda.py
and use `<benchmark tool="vereda">`. The tool directory holds vereda.py and
the binary with its models; pass `--clang` in the benchmark's options when
clang is not on PATH."""
from benchexec import result
import benchexec.tools.template
from benchexec.tools.sv_benchmarks_util import get_data_model_from_task, ILP32, LP64

# The SV-COMP verdict vereda.py prints on a line of its own under
# --property-file.
VERDICTS = {
    "TRUE": result.RESULT_TRUE_PROP,
    "FALSE_REACH": result.RESULT_FALSE_REACH,
    "FALSE_OVERFLOW": result.RESULT_FALSE_OVERFLOW,
    "UNKNOWN": result.RESULT_UNKNOWN,
}


class Tool(benchexec.tools.template.BaseTool2):
    """Vereda: bounded model checking of C through LLVM IR.
    https://github.com/mikhailramalho/vereda"""

    def executable(self, tool_locator):
        return tool_locator.find_executable("vereda.py")

    def name(self):
        return "Vereda"

    def project_url(self):
        return "https://github.com/mikhailramalho/vereda"

    def version(self, executable):
        # "Vereda 0.1.0 (LLVM 23.1.6, camada 0.25)"
        return self._version_from_tool(executable, line_prefix="Vereda ").split()[0]

    def cmdline(self, executable, options, task, rlimits):
        data_model = get_data_model_from_task(task, {ILP32: "-m32", LP64: "-m64"})
        if data_model:
            options = options + [data_model]
        if task.property_file:
            options = options + ["--property-file", task.property_file]
        return [executable] + options + list(task.input_files)

    def determine_result(self, run):
        for line in reversed(run.output):
            if line.strip() in VERDICTS:
                return VERDICTS[line.strip()]
        return result.RESULT_ERROR
