# ReachSafety baseline rerun after the wrong-verdict fixes

Recorded 2026-11-23.

The same 1407 tasks as the first baseline, on main at f53bf26d: with #60
(setjmp and longjmp give UNKNOWN), #61 (the null object's base), #62
(stack objects past the address space), #64 (uninitialised local arrays)
and #70 (the persistent map), and the same setup, 12 tasks at a time, 60 s
and 6 GB each. ESBMC's numbers are those of the first baseline, since its
binary did not change. Results are in
`~/tools/baselines/2026-11-23-reachsafety/vereda-rerun-f53bf26d.tsv`.

| Set | Tasks | First run correct | Rerun correct | ESBMC correct | Wrong, first → rerun |
|---|---|---|---|---|---|
| Arrays | 440 | 94 | 99 | 128 | 1 → 0 |
| ControlFlow | 73 | 28 | 8 | 40 | 14 → 0 |
| Heap | 136 | 118 | 119 | 118 | 9 → 0 |
| Loops | 758 | 344 | 343 | 418 | 0 → 0 |
| All | 1407 | 584 | 569 | 704 | 24 → 0 |

No wrong verdicts, which is the first condition of milestone 1's gate. The
correct count falls by 15, all explained: 20 longjmp tasks that walking
setjmp as a plain call answered right by luck (the same treatment gave 14
others wrong answers) are now UNKNOWN; `array_min_and_copy_shift_sum_add`
lost a TRUE that rested on an assumption LLVM had folded to false; and two
nla-digbench tasks at about 52 s went past 60 s while a third came in at
55 s, which is the load of five agents building on the machine during the
run. The six array-multidimensional tasks (#62) and `rule60_list2` (#61)
are new answers.

Of the 159 tasks ESBMC answers and Vereda does not, 107 stop at
`--max-k-step` 50 after a median of 0.5 s, and ESBMC answers 88 of those
within 10 s (#84); 32 are setjmp/longjmp programs (#11); 16 time out, and
the rest are unsupported constructs (a void type, calloc with a symbolic
size, a variadic function). Vereda alone answers 24 tasks.
