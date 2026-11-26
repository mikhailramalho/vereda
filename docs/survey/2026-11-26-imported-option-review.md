# Preserve semantic controls during option cleanup

Review of #269 found that the cleanup treated several semantic controls as
implementation switches. Keep --unwindset and --unwindsetname, including their
values, as unsupported requirements. Restore those commands from main's original
fixtures and undo promotions that depended on discarding their bounds.

Canonical spelling is independent of support. Normalize --data-races-check and
--no-standard-check through a separate unsupported-alias table, retaining Needs
and FUTURE metadata. Preserve explicit boolean values. The binary's explicit
error for an unimplemented check does not constitute support for verification.

Restore --cvc5 as --solver cvc5, consistent with --cvc. Preserve
-D__builtin_unreachable() through the ordinary preprocessor-definition rule:
making that builtin a no-op changes the program's semantics.

Regenerate affected metadata from main's original RUN commands through the shared
header/translate path. Once no unsupported requirement remains, this also
translates raw --z3, --bitwuzla and --overflow left in former fallback commands.
Keep unsupported commands visible and recheck modes rather than inferring success
from an empty Needs line. Regression coverage checks both the semantic controls
and translation after dropping the last internal-only blocker.

Current main (63378e92, #257) is already merged into the branch; no unresolved
merge entries remain. This correction adds no binary options or mode flags.

Validation: 777 regenerated fixtures finish with 184 passes, 593 skips and no
failures using three workers and a 15-second per-run limit. Timeout cases retain
their modes; full 300-second coverage remains for CI. Importer regressions and
all 14 configuration tests pass. Audits confirm unchanged retained C bodies and
no stranded raw --z3/--bitwuzla/--overflow flags in regenerated commands without
Needs metadata.

Follow-up issues: #310.
