# Disjoint field storage for struct arrays

The mixed-width tuple table stores kind and len as interleaved bytes. That
prevents the existing whole-word array path and hides the independent periodic
and affine sequences from tableFunction. Partition eligible fields in memory,
using the existing owned-array machinery for each column.

An IR split was considered, but keeping the split in memory preserves source
object identity, offsets, read-only checks and trace locations. The source
object retains its original size. Field partitions use private storage IDs with
base zero and exists=false: they consume no program address space and are never
returned as program pointers. Integer fields use word arrays; fields that may
carry pointers retain byte provenance. The source backing retains padding and
unobserved fields.

For stride 2^s, field offset f and width w, byte f+d+i*stride maps to byte d+i*w
of its partition. Compute the source offset modulo the original pointer width
before shifting, preserving wrapped GEP arithmetic. The mapping is injective
within each disjoint column because w <= stride. A small exact fold for extracts
of known-zero low bits recognizes aligned sums/products without solver queries.

Eligibility uses the existing non-capture proof and additionally requires no
address exposure. It covers global arrays, fixed stack arrays and dynamic
allocas of structs. Initially accept power-of-two struct strides 2..64, scalar
integer/pointer accesses of 1/2/4/8 bytes, up to four GEP levels and four dynamic
index contributions, each scaled by a multiple of the struct stride. Field
intervals must be disjoint; incompatible widths or pointer/integer observations
at the same offset reject the split. Other uses keep the existing byte model.
This includes byte overlap, escaping addresses, unsupported address spaces,
pointer PHIs/selects and partial copies/fills. Heap objects are unchanged.

Whole-object zeroing is supported. Whole-object copies up to MaxBytes preserve
every byte; matching layouts copy partitions directly and copy the backing for
padding/unobserved fields. Other layouts use the byte-copy path with projected
reads/writes. Reuse the copy classifier's existing rejection of used return
values, which introduce aliases outside the direct-use graph.
Initialisers and copies can carry pointer provenance even when observed through
integer fields, so inspect underlying pointer types and conservatively retain
byte provenance for copies whose source is not proved pointer-free.

The existing CellMap state copies and joins work unchanged because each private
partition is an ordinary owned array. Whole-object fill/havoc reaches the
partitions as well as the backing. Pointer reachability attributes provenance in
partitions to the source owner. Single-store loop summaries project their first
address and stride into the chosen field. Loops storing several fields still
unroll under the existing one-store summary restriction, then compress each
field independently. No flags are added.

## Measurements and validation

Use LLVM 23.1.6, rustc 1.100.0, camada/Bitwuzla from the existing dependency build,
and the normal 4 GiB regression limit. The earlier #273 binary has the same src/
tree as main aebbf301 (its version banner predates its commit). With the same
head fixture and frontend, that binary reproduces an OOM on
tuple_const_array_member_reads. The new binary passes with 0.421 seconds wall,
0.020 seconds walk, 0.215 seconds solve and 33 MB peak. Promote the fixture to CORE.
The failing variant reports both expected failures in 0.192 seconds wall and
34 MB peak in the initial comparison. The nested one-byte struct table remains
on the existing byte-periodic path; remove its stale importer OOM classification.

The initial full run completes 537 Vereda cases (536 pass, one known-bug skip)
and 6,140 imported cases (2,590 pass, 3,550 skip), with no failures. Add returned
copy-alias and pointer-vector initializer regressions to verify the existing
alias exclusion and the new provenance handling explicitly.
The final Vereda run completes all 539 cases in 22.92 seconds: 538 pass, the
existing lib_sprintf_overflow case skips, and none fail.

New cases cover mutable fields on 32/64-bit targets, symbolic stores, branch
joins, whole copies/zeroing, pointer copies through integer layouts, read-only
writes, VLAs, single-field loop summaries, failing properties, byte-overlap and
escape fallbacks, returned copy aliases and non-inbounds GEP wrap. Reimporting
the affected constant tables retains CORE status. Full CI remains pending.

Follow-up issues: #317.
