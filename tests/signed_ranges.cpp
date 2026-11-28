// Signed ranges through deep add, sub and mul chains: a comparison folds as
// it is built when a sum of sign-extended bytes 20 deep stays below it, a
// chain that uses each term twice 30 deep is settled in linear time, under
// a path's bounds a sum decides without leaving those bounds behind, and
// sums 40 deep, past any fixed depth of 32, are bounded on every path when
// first queried at their top and under a path's bounds, signed and unsigned,
// and a level inside such a sum keeps its range after a query at the top.
#undef NDEBUG
#include "encode/terms.h"
#include "solver/solver.h"
#include <cassert>
#include <optional>
#include <string>

using vereda::encode::Term;
using vereda::encode::Terms;

int main() {
  auto solver = vereda::solver::create(camada::SolverKind::Bitwuzla);
  assert(solver);
  Terms t(**solver);
  auto byte = (*solver)->mkBVSort(8), word = (*solver)->mkBVSort(32);

  // 20 bytes in [-128, 127] sum to [-2560, 2540].
  Term sum = t.sext(t.symbol("b0", byte), 24);
  for (int i = 1; i < 20; ++i)
    sum = t.add(sum, t.sext(t.symbol("b" + std::to_string(i), byte), 24));
  assert(t.isTrue(t.slt(sum, t.bv(2541, 32))));
  assert(t.isTrue(t.sle(t.bv(-2560, 32), sum)));
  assert(!t.isTrue(t.slt(sum, t.bv(2540, 32))));

  // d(k+1) = d(k) + (d(k) - 1) from a byte: below 2^38 after 30 steps. Each
  // level reads d(k) twice, so without a cache this is 2^30 calls.
  Term d = t.sext(t.symbol("d", byte), 56);
  for (int i = 0; i < 30; ++i)
    d = t.add(d, t.sub(d, t.bv(1, 64)));
  assert(t.isTrue(t.slt(d, t.bv(int64_t(1) << 38, 64))));

  // Under bounds [-10, 10] for each of 20 words, their sum is below 201;
  // without them it may be anything.
  Term w = t.symbol("w0", word);
  std::vector<Term> words{w};
  for (int i = 1; i < 20; ++i) {
    words.push_back(t.symbol("w" + std::to_string(i), word));
    w = t.add(w, words.back());
  }
  auto known = [&](const Term &x) -> std::optional<Terms::Bounds> {
    for (const Term &v : words)
      if (x == v)
        return Terms::Bounds{0, 0xffffffffu, -10, 10};
    return std::nullopt;
  };
  Term below = t.slt(w, t.bv(201, 32));
  assert(!t.isTrue(below));
  assert(t.decide(below, known) == 1);
  assert(t.decide(t.slt(w, t.bv(200, 32)), known) == -1);
  assert(!t.isTrue(t.sle(w, t.bv(200, 32))));
  assert(t.decide(t.sle(w, t.bv(200, 32)), {}) == -1);

  // 40 bytes in [-128, 127], summed with no query in between, sum to
  // [-5120, 5080].
  Term deep = t.sext(t.symbol("c0", byte), 24);
  for (int i = 1; i < 40; ++i)
    deep = t.add(deep, t.sext(t.symbol("c" + std::to_string(i), byte), 24));
  assert(t.isTrue(t.slt(deep, t.bv(5081, 32))));
  assert(!t.isTrue(t.slt(deep, t.bv(5080, 32))));

  // Under bounds [-10, 10] for each of 40 words, their sum is below 401.
  Term v = t.symbol("v0", word);
  std::vector<Term> vs{v};
  for (int i = 1; i < 40; ++i) {
    vs.push_back(t.symbol("v" + std::to_string(i), word));
    v = t.add(v, vs.back());
  }
  auto knownV = [&](const Term &x) -> std::optional<Terms::Bounds> {
    for (const Term &u : vs)
      if (x == u)
        return Terms::Bounds{0, 0xffffffffu, -10, 10};
    return std::nullopt;
  };
  assert(t.decide(t.slt(v, t.bv(401, 32)), knownV) == 1);
  assert(t.decide(t.slt(v, t.bv(400, 32)), knownV) == -1);
  assert(t.decide(t.slt(v, t.bv(401, 32)), {}) == -1);

  // 40 bytes zero-extended, summed with no query in between, sum to at most
  // 10200; asked after the top, the sum of the first 20 is at most 5100.
  Term up = t.zext(t.symbol("e0", byte), 24), half;
  for (int i = 1; i < 40; ++i) {
    up = t.add(up, t.zext(t.symbol("e" + std::to_string(i), byte), 24));
    if (i == 19)
      half = up;
  }
  assert(t.isTrue(t.ult(up, t.bv(10201, 32))));
  assert(!t.isTrue(t.ult(up, t.bv(10200, 32))));
  assert(t.isTrue(t.ult(half, t.bv(5101, 32))));
  assert(!t.isTrue(t.ult(half, t.bv(5100, 32))));

  // Under bounds [0, 10] for each of 40 words, their sum is below 401.
  Term u = t.symbol("u0", word);
  std::vector<Term> us{u};
  for (int i = 1; i < 40; ++i) {
    us.push_back(t.symbol("u" + std::to_string(i), word));
    u = t.add(u, us.back());
  }
  auto knownU = [&](const Term &x) -> std::optional<Terms::Bounds> {
    for (const Term &y : us)
      if (x == y)
        return Terms::Bounds{0, 10, 0, 10};
    return std::nullopt;
  };
  assert(t.decide(t.ult(u, t.bv(401, 32)), knownU) == 1);
  assert(t.decide(t.ult(u, t.bv(400, 32)), knownU) == -1);
  assert(t.decide(t.ult(u, t.bv(401, 32)), {}) == -1);
  return 0;
}
