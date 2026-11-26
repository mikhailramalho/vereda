# Small-domain symbolic load folding in Memory::load (#279)

## Motivation

`tests/esbmc/gcse.c` spent ~61 s in Bitwuzla despite having no symbolic loops.
Its `IP_INST` struct mixes `UINT8 PortIsDisabled[4]` with `void *` pointer
arrays, so `pInst` stayed a flat byte array. Reading
`pInst->RegCntxtPortList1[aux_index]` at bounded `aux_index < 4` did not fold
to `&m_PORT_RegMapTable` (even though all four entries hold that exact pointer),
producing a pointer with a symbolic object ID. Dereferencing it flattened all
exposed globals into the 96-bit global `memory#0` array (2,003 byte stores) and
prevented `RegisterHandler == NULL` from folding, triggering three `havoc#`
arrays via `__vereda_unknown_target`.

## Design

Expose `Terms::range(const Ref &e, Known known = {})` (clearing `scratch` when
path bounds are supplied) and check it in `Memory::load` before `prepare()`
flushes `st.known`:

- When loading from a constant object ID at a non-constant offset whose
  strided interval `Terms::Range{lo, hi, stride}` has at most 16 candidate
  positions (`count <= 16`) and every candidate byte is present in `st.known`:
  - Check all candidate bytes first (pure lookup, no side effects).
  - If all candidates have identical byte values (and identical provenance for
    pointer loads), decode and return that single value directly without
    flushing `st.known` or emitting array `select`s.
  - Otherwise, for pointer loads with at most 8 candidates (`count <= 8`),
    emit an `ite` chain over the decoded pointers so each arm keeps its known
    object ID and avoids flattening into global `memory#0`.

## Measurements

On LLVM 23.1.6 / Bitwuzla 0.11.0, `tests/esbmc/gcse.c`:

| Metric | Before (`main`) | After (#279) |
|---|---:|---:|
| Walk time | 0.045 s (3 checks, 46 MB) | 0.004 s (1 check, 36 MB) |
| Solve time | ~61.0 s | 0.027 s (44 MB) |
| Wall time | 61.05 s | **0.17 s** (~359x faster) |
| `memory#0` / `havoc#` | 2,003 stores, 3 `havoc#` | 0 stores, 0 `havoc#` |
