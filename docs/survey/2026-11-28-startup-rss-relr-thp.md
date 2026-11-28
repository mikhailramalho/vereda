# Startup memory: -z pack-relative-relocs and MIMALLOC_ALLOW_THP=0

Recorded 2026-11-28. Follow-up to 2026-11-24-lld-startup-memory.md (#318).

Measured on the second machine (Core Ultra 9 275HX, glibc 2.43, Release
builds with the pinned LLVM 23.1.6 clang and lld), every run on its
performance cores (CPUs 0-7):

- base: main at 3100ef93;
- pack: the same with `LINKER:-z,pack-relative-relocs` next to
  `-z keep-text-section-prefix` in CMakeLists.txt;
- thp0: base run with `MIMALLOC_ALLOW_THP=0` in the environment.

With pack, `.rela.dyn` shrinks from 1.98 MB to 0.11 MB plus a 12.7 KB
`.relr.dyn`, and the binary from 136.6 MB to 134.8 MB.

## Startup

`vereda --version`, ten runs each on one core, peak RSS by `/usr/bin/time`
(MB here are 1,000 of its kB):

| | median | range |
|---|---:|---:|
| base | 25.4 MB | 23.6-26.0 |
| pack | 23.5 MB | 21.6-24.0 |
| thp0 | 21.8 MB | 21.6-22.3 |
| pack and thp0 | 20.0 MB | 18.0-20.4 |

The two savings add up: about 1.9 MB from the packed relocations and 3.6 MB
from mimalloc not backing its first arena with huge pages. (The main machine
measured 34 MB for base on 2026-11-24; this one's kernel and glibc map less.)

## Both suites

`ctest -R '^(esbmc|vereda/)' -j8` on CPUs 0-7 with `REGRESS_STATS`, two runs
of each configuration, alternating. Sums over the 7,021 of ctest's 7,432
tests that record statistics (the rest are unit tests and skipped ones);
peak memory is per test:

| | wall | walk | solve | median peak | sum of peaks | ctest wall |
|---|---:|---:|---:|---:|---:|---:|
| base | 586.7, 586.6 s | 26.9, 27.4 s | 76.8, 75.4 s | 39 MB | 185.9, 185.2 GB | 107.7, 108.6 s |
| pack | 582.2, 575.9 s | 26.3, 26.9 s | 76.2, 75.2 s | 37 MB | 177.1, 176.0 GB | 107.1, 106.6 s |
| thp0 | 581.3, 582.4 s | 27.9, 27.7 s | 78.4, 78.5 s | 33 MB | 157.2, 156.6 GB | 107.2, 107.3 s |

No verdict changed. With pack no test moved beyond noise: the largest
changes are 32_floppy 26.8 s to 26.6 s, and 5 MB more peak on a few
30 MB runs. With thp0 every short run is 6 MB smaller, and 32_floppy's peak
goes from 1,018 MB to 992 MB, but the long runs are slower:
mem_store_chain_reads 4.5 s to 4.8 s, fmod_nondet 8.0 s to 8.3 s,
32_floppy 26.8 s to 27.3 s, and the walk and solve sums 2.5% and 3% higher.

## Decision

Neither is adopted.

- `MIMALLOC_ALLOW_THP=0` buys 6 MB per run, which no memory limit is near
  on the short runs it helps, for 2.5-3% more walk and solve time on the
  long runs, the ones that decide timeouts. A user short of memory can set
  it in the environment.
- `-z pack-relative-relocs` is free in time and saves about 2 MB per run,
  but the binary then needs glibc 2.36 or later where it runs (DT_RELR);
  built with lld on an older system, it would not start. CI and SV-COMP run
  Ubuntu 24.04 (glibc 2.39), so it would cost nothing there, but 2 MB does
  not pay for a glibc floor or a configure-time check of the C library.
  Revisit if the startup RSS matters again, for instance for many short
  parallel runs.
