# Per-loop unwind bounds

Recorded 2026-11-28.

--unwindsetname name:idx:nr,... now gives loop idx of function name its
own bound in place of --unwind, as ESBMC's flag does (issue #310). A
loop's index is the number its unwinding assertion prints, counted from 0
in each function; a name may be clang's USR (c:@F@name#), as ESBMC also
takes, or file.c@name. The bound applies to the loops the walker unrolls;
a loop replaced by its summary has none, as under --unwind.
--incremental-bmc rejects the flag, since it picks the bounds itself.

ESBMC numbers a function's loops in the order of their backward gotos, so
an inner loop comes before its outer one, while Vereda numbers loop
headers in reverse post-order, outer first. The two agree for loops that
are not nested, as in the eight imported tests that use the flag.

Not adopted: --unwindset L:nr. ESBMC's L is a loop number over its whole
goto program, its C library's loops included (in the imported tests a
test's first loop is loop 3, after two library loops), which no Vereda
numbering can match; the six imported tests that use it stay FUTURE.
--no-standard-checks is left for a change of its own: in Vereda's terms it
is --ub-pointer-check=false --ub-div-by-zero-check=false
--vla-size-check=false (ESBMC's other standard checks, bounds, pointer
relations, alignment and unlimited scanf, are ones Vereda does not run by
default), and 43 imported tests need it and no other flag.
