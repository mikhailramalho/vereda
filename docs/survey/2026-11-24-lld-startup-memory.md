# Static constructors and lld's text layout

Recorded 2026-11-24.

The ReachSafety rerun (`2026-11-24-reachsafety-rerun-on-a453547a.md`, #172)
found the Release binary that `-DDOWNLOAD_DEPENDENCIES=ON` builds (installed
LLVM 21.1.2, camada 0.24 with Bitwuzla, Z3 and cvc5, mimalloc 3.7.0, linked
by lld) at 79 MB resident on `vereda --version`, against 34 MB for the same
commit linked by ld.bfd 2.45.

## Where the memory went

`/proc/<pid>/smaps` at `exit_group` (under gdb) on `--version`, the
mappings that differ:

| Mapping | lld | ld.bfd |
|---|---:|---:|
| text (`r-xp`) | 63,064 kB | 17,720 kB |
| read-only data (`r--p`, before text; after it too for ld.bfd) | 4,400 kB | 2,300 + 1,800 kB |
| anonymous, all mappings | 7,868 kB | 7,868 kB |
| total | 79,924 kB | 34,280 kB |

The 45 MB are pages of the binary's code, mapped from the page cache. The
anonymous memory is the same with both: mimalloc's 64 MiB arena reserve
holds 6,144 kB in three transparent huge pages with both, the relocated
`.data.rel.ro` 836 kB and `.data`/`.bss` 456 kB. The 2.2 MB of `.rela.dyn`
(96,000 relocations) and the sizes of `.text` (93 MB) are the same
too, so neither relocations, mimalloc's start nor folding or dropping
sections (neither link uses `--icf` or `--gc-sections`) make the
difference.

What does is where the static constructors sit. The binary has 1,789 of
them (`_GLOBAL__sub_I_*`, one per source file with globals to construct):
885 from Z3, 553 from cvc5, 339 from LLVM, 7 from Bitwuzla and 5 more.
The compilers put each in a `.text.startup` section. ld.bfd's default
script gathers all of those in one run near the start of `.text`, after
`.text.unlikely` and `.text.exit`; lld leaves each with the rest of its
object's code unless given `-z keep-text-section-prefix`. Counted from
lld's link map, the 1,789 constructors fall into 921 different 64 KiB
windows of `.text` (Z3's into 422, cvc5's into 289, LLVM's into 199); in
ld.bfd's into 23. Running each one faults in its page, and the kernel's
fault-around maps the whole 64 KiB window around it from the page cache,
so lld's binary maps about 58 MB of code before `main`.

## The fix

`-z keep-text-section-prefix` makes lld place `.text.startup`,
`.text.unlikely`, `.text.hot` and `.text.exit` input sections in output
sections of those names, as ld.bfd's script does. With it the
constructors sit in one 1.27 MB `.text.startup`, in 23 windows, and the
text mapping holds 18,072 kB on `--version`. CMakeLists.txt adds it to the
link of `vereda` when CMake reports lld as the linker
(`CMAKE_CXX_COMPILER_LINKER_ID`, CMake 3.29 and later), which covers both
`DOWNLOAD_DEPENDENCIES` paths and perf.yml's base build, which names lld
itself. ld.bfd ignores the option with a warning, so it does not get it.

## Measurements

Release builds of main at 8f2b8df1 as above: lld without the option
(before), lld with it (after), ld.bfd. The binary alone on IR that clang
compiled beforehand, three runs each, interleaved, peak RSS by
`/usr/bin/time` (MB here are 1,000 of its kB); wall time by hyperfine (30
runs) where it is under 0.1 s. The machine has 80 cores and ran under a
load of about 19.

| Case | before | after | ld.bfd |
|---|---:|---:|---:|
| `--version`, RSS | 78.5 MB | 34.3 MB | 34.4 MB |
| `--version`, wall | 15.1 ms | 10.0 ms | 10.9 ms |
| loop_nested `--unwind 5`, RSS | 89.0 MB | 46.7 MB | 46.1 MB |
| loop_nested `--unwind 5`, wall | 22.5 ms | 16.6 ms | 17.8 ms |
| array_loop_summary_decrement_fail, RSS | 90.4 MB | 48.0 MB | 47.5 MB |
| array_loop_summary_decrement_fail, wall | 29.0 ms | 19.7 ms | 20.4 ms |
| same, `--no-summarize-array-loops`, RSS | 1,018.7 MB | 974.8 MB | 972.9 MB |
| same, `--no-summarize-array-loops`, wall | 9.15, 9.46, 9.27 s | 9.11, 9.28, 9.34 s | 10.00, 9.34, 9.58 s |

RSS is the median of three (runs differed by at most 2.1 MB). The fix
takes 42 to 44 MB off every run, and 5 to 9 ms off each short one, 4 to 8
ms of it system time, since the kernel maps 11,000 fewer pages; on the long
run the time does not change beyond the noise.

## Not done

Two more megabytes come off with `-z pack-relative-relocs` (measured with
lld; ld.bfd has it from 2.38): `.rela.dyn` shrinks from 2.2 MB to 0.2 MB
plus a 14 KB `.relr.dyn`, and `--version` takes 31 to 33 MB. The binary then
needs glibc 2.36 or later where it runs.

mimalloc's CMake turns on transparent huge pages for its arenas on Linux
(`MI_ALLOW_THP` FULL). With `MIMALLOC_ALLOW_THP=0` in the environment,
`--version` takes 28.4 to 29.1 MB instead of 34.3 MB; what that costs in
time on long runs is not measured.
