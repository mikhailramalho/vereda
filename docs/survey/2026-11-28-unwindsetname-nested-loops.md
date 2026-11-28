# Loop numbers for --unwindsetname

Recorded 2026-11-28.

ESBMC's --unwindsetname name:idx:nr counts a function's loops in the
order of their back edges in its goto program (`esbmc --show-loops`
lists them as main:0, main:1, ...): an inner loop comes before the loop
around it, and sibling loops, those in the two branches of an if among
them, come in program order. Vereda numbered loops by their headers in
reverse post-order, which put an outer loop first and an inner loop
after the loops that follow it. It now numbers them as ESBMC does, in a
post-order of the loop nest with siblings in program order (LLVM's
getLoopsInReverseSiblingPreorder, reversed). The unwinding assertions
and progress lines print the same numbers.

Translating the index in the importer instead was not possible: it
would need the loop structure of each C source.

On a function with a loop in each branch of an if, a loop holding two
others and a do-while after them, all six indices now select the loop
ESBMC's do. No imported test changed verdict; one Vereda test that
named loops by number in its progress lines was renumbered.
