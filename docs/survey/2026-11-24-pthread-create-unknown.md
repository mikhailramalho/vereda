# A reached pthread_create gives UNKNOWN (#112)

The walker treated `pthread_create` as any bodyless call, so the thread
function never ran and a violation only a thread reaches gave a wrong
SUCCESSFUL. Until threads are modelled (#49), a reached `pthread_create` now
throws `Unsupported`. #111 had also let no `pthread_*` call write through its
arguments; only the mutex and condition-variable calls (init, lock, trylock,
unlock, destroy, wait, signal, broadcast) keep that, since they write only the
lock or condition, which the program never reads, while getters such as
`pthread_attr_getstacksize` write their out-parameter.

## Imported tests

Of the 677 imported tests that call a `pthread_` function, 232 give a verdict
(all but 7 of the rest need a flag Vereda lacks), and 218 of those now give
UNKNOWN, every one at a `pthread_create`:

- 104 had the expected verdict (88 CORE, 15 THOROUGH, 1 FUTURE that is FUTURE
  for another reason): lost. The 103 CORE and THOROUGH ones are now FUTURE.
- 114 had the wrong one (112 FUTURE, 2 KNOWNBUG; 97 of them SUCCESSFUL where
  the test expects FAILED): fixed, in that they no longer give a wrong answer.

No other imported test changed verdict.

## SV-COMP

None of the 1,407 tasks of the 2026-11-23 ReachSafety run (Arrays,
ControlFlow, Heap, Loops) calls a `pthread_` function; the word appears only
in the headers' typedefs. ConcurrencySafety, where every task creates
threads, was not part of that run.
