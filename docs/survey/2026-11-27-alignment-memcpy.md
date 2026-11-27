# Misaligned accesses clang lowers with align 1

Recorded 2026-11-27.

## What clang does

`--ub-alignment-check` checks each typed load and store against the
alignment it carries in the IR. ESBMC's github_10891_unpacked_align_fail
reads `*(struct N *)((unsigned char *)buf + o)` and Vereda said SUCCESSFUL
(#337). The copy is a `memcpy` with `align 1` on the source, but memcpy is
not the cause: when an explicit pointer cast's operand is derived from a
declared object, clang keeps the alignment it can prove from the
declaration and the offsets instead of the cast-to type's
(`EmitPointerWithAlignment` in clang's `CGExpr.cpp`, "if the source l-value
is opaque, honor the alignment of the casted-to type"). Measured with clang
23 at -O0:

| source | IR |
|---|---|
| `*(uint32_t *)((char *)buf + o)`, buf a local array | `load i32, align 1` |
| `*(uint32_t *)((char *)buf + 1)` | `load i32, align 1` |
| `struct N r = *(struct N *)((char *)buf + o)` | `memcpy(align 4 %r, align 1 %p, 4)` |
| `*(struct N *)((char *)buf + o) = n` | `memcpy(align 1 %p, align 4 %n, 4)` |
| `((struct N *)((char *)buf + o))->v` | struct GEP, `load i32, align 1` |
| same through a pointer parameter `q` | `align 4` (q is opaque) |
| `memcpy(&r, (char *)buf + o, 4)` | identical to the struct copy above |
| `__builtin_memcpy(...)` | identical to the struct copy above |
| packed member `p->x` | struct GEP into a packed struct type, `align 1` |

`-fsanitize=alignment` takes the same lvalue alignment
(`EmitTypeCheck` uses `LV.getAlignment()`), so it misses every row but the
member access; checking in the front end with it does not help, and no
clang flag turns the declaration rule off.

## Fix

The type's alignment is recovered in `checkAlignments` (`src/ir/load.cpp`),
which runs on clang's unoptimised IR before mem2reg. A pointer that is a
chain of array or pointer-arithmetic GEPs from an alloca or a global is the
shape clang gives a cast of a declared object's address
(`castFromDeclaration`). A load or store through it needs its IR type's ABI
alignment. A memcpy or memmove through it needs the alignment of the
aggregate on the other side when that side (an alloca, a global or a GEP)
points to an aggregate of the copied size: the type's ABI alignment, or the
other side's own alignment when clang gave it less, as it does for a packed
struct or union whose layout is natural (`struct __attribute__((packed))
{ uint32_t v; }` is `{ i32 }` in the IR).

The front end passes `-fno-builtin-memcpy -fno-builtin-memmove` with
`--ub-alignment-check`, so a memcpy or memmove the user calls stays a library
call, which the walker already handles and which is never checked: copying
bytes is the way C reads unaligned data. Only modules whose `llvm.ident`
names clang get the rule; Rust reads packed fields through byte GEPs, and
the models linked later have been through InstCombine, which rewrites struct
GEPs as byte GEPs.

Clang's own accesses below the type's alignment are left out, each found by
running the check on programs that use them:

- a GEP into a struct field: packed members, and the argument coercions
  that store `i24` or `i56` with `align 1` into a struct of a `char` array;
- a byte offset from a struct: bitfields of a packed struct
  (`load i16, ptr getelementptr (i8, ptr @d, i64 1), align 1`,
  ESBMC's struct_bitfields_07);
- a GEP whose result is a struct: a member of an array of packed unions,
  which clang reads with no field GEP (`%union.U = type { i32 }`,
  `load i32, align 1`).

Struct copies and coercions between allocas involve no GEP. x86-64 and i386
struct passing and returning, unions, bitfields, complex numbers and
`va_arg` gave no new failing check.

ESBMC's tests that expect VERIFICATION SUCCESSFUL (2,603 under `tests/esbmc*`)
were run with `--ub-alignment-check` before and after. Two more fail an
alignment check: deref_union_array_read and deref_union_array_write copy
`*(union U *)b` with `b` an `unsigned char[8]`, offset 0. ESBMC takes a
local array's base as aligned; Vereda gives an object its declared
alignment, as for `*(int *)c` with `char c[4]`, which `tests/alignment_check.py`
already expects to fail. Neither runs with the flag in the suite.

## Known gaps

`__builtin_memcpy` into an aggregate of exactly the copied size, from a
misaligned cast of a declared object, is now reported: the IR is the same as
a struct copy's, and `-fno-builtin` leaves `__builtin_memcpy` an intrinsic.
Into a scalar it is not. A scalar typedef with `__attribute__((aligned(1)))`
read through such a cast is reported, as ESBMC 8.8 does.

Still unchecked: a member read through the cast
(`((struct N *)(buf + o))->v`, a field GEP), a cast of a struct's address
(`*(int *)((char *)&s + 1)`, a byte offset from a struct), and a struct copy
whose other side is neither an alloca, a global nor a GEP
(`*(struct N *)(buf + o) = *q`).

`github_10891_unpacked_align_fail` gives FAILED with `--ub-alignment-check`
but stays FUTURE: ESBMC checks alignment by default and the imported test
has no flag, while Vereda's check is opt-in.

## Not adopted

`-fsanitize=alignment`: it takes the same lvalue alignment and misses the
same accesses. Recovering the type from debug info: neither a load nor a
memcpy carries its source type, and the user's memcpy and clang's struct
copy have the same debug location shape. Inserting checks from clang's AST
(`-ast-dump=json`) would need a second compile and a source-location map
onto the IR.

Follow-up issues: #352, #353.
