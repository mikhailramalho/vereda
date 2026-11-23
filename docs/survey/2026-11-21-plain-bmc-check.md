# Plain-BMC check

Recorded 2026-11-21.

ESBMC rerun with the competition's reach flags, `--incremental-bmc` instead
of k-induction, 15 GB and 900 s per task, each in its own systemd unit:

| Task | ESBMC, plain BMC | Kept |
|---|---|---|
| array_assert_loop_dep | FAILED at k = 1, 8 s, 510 MB | no |
| tdh_sys_config | UNKNOWN at k = 50, 10 s, 216 MB | no |
| sanfoundry_02_ground | out of memory at k = 1 | yes |
| Problem14 | out of memory at k = 1 | yes |
| SpamAssassin-loop | out of memory at k = 3 | yes |
| pals_floodmax | out of memory at k = 2 | no: needs function pointers |
| uthash_JEN_nondet_test1-1 | timeout at k = 1 | yes |
| Problem08_label00, Problem102_label00 | timeout at k = 6 | yes |
| bAnd4 | timeout at k = 22 | yes |

Two of ten blew up only under k-induction. The spike set is the seven kept
tasks.
