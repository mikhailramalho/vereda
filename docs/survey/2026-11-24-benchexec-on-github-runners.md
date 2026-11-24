# BenchExec on GitHub's runners

Recorded 2026-11-24.

`.github/workflows/benchexec.yml` runs Vereda on ReachSafety categories with
BenchExec 3.37 on GitHub's hosted `ubuntu-24.04` runners, started by hand,
to catch wrong and lost verdicts. It is modelled on ESBMC's
`dispatch-benchexec.yml`, which builds on GitHub but runs on a machine of
its own with sv-benchmarks cloned there.

The runners the test runs got were KVM guests on AWS (kernel 7.0.0-aws), an
Intel Xeon 6975P-C with 2 cores of 2 threads, 16.4 GB of memory and 2 GB of
swap. The build job takes about a minute (21 s of apt, 34 s to build), and a
run job about 25 s before BenchExec starts, of which 16 s install clang 21
and BenchExec. Four run jobs started at once.

Containers work, given two things. Ubuntu's AppArmor denies unprivileged
user namespaces there (`kernel.apparmor_restrict_unprivileged_userns` is
1): `containerexec` fails with "Unprivileged user namespaces are forbidden
on this system", and works once the sysctl is 0. And BenchExec needs a
cgroup of its own with the cpuset, cpu, io, memory and pids controllers.
The steps run in `/system.slice/hosted-compute-agent.service`, BenchExec
from pip has no pystemd to ask systemd for a scope, and
`python3 -m benchexec.check_cgroups` finds no usable cgroup. The runner's
user does have a systemd manager (no login session, but
`systemd-run --user --scope -p Delegate=yes` starts a scope), but systemd
delegates no cpuset to user managers by default, so that route needs a
drop-in for `user@.service` and a restart; it was not tried. Instead the
job makes an empty `/sys/fs/cgroup/benchexec`, owned by the runner's user,
with every controller enabled at the root, and the run step moves its own
shell there with sudo and execs BenchExec, which finds itself the only
process in its cgroup and uses it, as its documentation asks of machines
without systemd. The tools then run with the file system read-only, a
/tmp of their own and an overlay on /home, where BenchExec puts the
container's home directory. Each run gets 2 threads of one core and 6 GB,
two at a time.

sv-benchmarks comes from GitLab as a partial clone (`--filter=blob:none
--depth 1`) with a sparse checkout of the set files, the properties and the
directories the categories' set files name: about 50 MB for Arrays,
ControlFlow, Heap and Loops, and the whole plan job takes about 20 s. That
is cheap enough not to cache; the plan job fetches it once and hands it to
the run jobs as an artifact. GitLab serves a commit by its hash as well as
a branch or tag.

BenchExec joins a relative path in a run definition to the definition's
directory before it expands variables, so `$SV_BENCHMARKS/c/...` in an
`includesfile` matched nothing when the definition was not in the working
directory; the definition names `../../sv-benchmarks` instead. BenchExec
exits 0 when it ran no task at all, so the run job checks for a results
file. The categories are SV-COMP's as ESBMC's run definition gives them, so
Heap includes LinkedLists and Loops includes VerifyThis-Loops: 440, 73, 264
and 764 tasks, against 440, 73, 136 and 758 in the baselines of 2026-11-23.
