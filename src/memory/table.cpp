#include "memory/table.h"
#include <algorithm>
#include <bit>
#include <optional>
#include <vector>

using vereda::encode::Term;
using vereda::encode::Terms;
using llvm::APInt;

namespace vereda::memory {
namespace {

constexpr size_t MaxExceptions = 8;
constexpr size_t MaxSegments = 8;
constexpr size_t MinSegment = 8;

Term resized(Terms &t, const Term &j, unsigned w) {
  return j.width() > w ? t.extract(j, w - 1, 0)
         : j.width() < w ? t.zext(j, w - j.width()) : j;
}

std::optional<APInt> difference(Terms &t, const Term &a, const Term &b) {
  const APInt *ca = t.constant(a), *cb = t.constant(b);
  if (ca && cb)
    return *ca - *cb;
  if (const APInt *d = t.constant(t.sub(a, b)))
    return *d;
  return std::nullopt;
}

struct Line {
  Term first, step;
  size_t lo, hi;
  Term at(Terms &t, const Term &j) const {
    Term k = t.sub(resized(t, j, first.width()),
                   t.bv(APInt(64, lo).zextOrTrunc(first.width())));
    return t.add(first, t.mul(step, k));
  }
};

Terms::Def affine(Terms &t, std::span<const Term> values) {
  unsigned w = values[0].width();
  size_t limit = std::min(MaxExceptions, values.size() / 16);
  std::vector<std::pair<Term, Term>> tried;
  // Eight exceptions can spoil at most sixteen adjacent pairs.
  for (size_t a = 0; a < std::min(values.size() - 1, 2 * limit + 1); ++a) {
    auto d = difference(t, values[a + 1], values[a]);
    if (!d || d->isZero())
      continue;
    APInt offset = *d * APInt(64, a).zextOrTrunc(w);
    Term first = t.sub(values[a], t.bv(offset)), step = t.bv(*d);
    std::pair<Term, Term> key{first, step};
    if (std::find(tried.begin(), tried.end(), key) != tried.end())
      continue;
    tried.push_back(key);
    std::vector<std::pair<size_t, Term>> exceptions;
    APInt expected = -offset;
    for (size_t i = 0; i < values.size(); ++i) {
      auto actual = difference(t, values[i], values[a]);
      if (!actual || *actual != expected) {
        exceptions.emplace_back(i, values[i]);
        if (exceptions.size() > limit)
          break;
      }
      expected += *d;
    }
    if (exceptions.size() <= limit) {
      return [&t, line = Line{values[a], step, a, values.size()},
              exceptions = std::move(exceptions)](const Term &j) {
        Term r = line.at(t, j);
        for (const auto &[i, v] : exceptions)
          r = t.ite(t.eq(j, t.bv(APInt(j.width(), i))), v, r);
        return r;
      };
    }
  }
  return {};
}

Terms::Def periodic(Terms &t, std::span<const Term> values) {
  for (unsigned bits = 1; bits <= 3; ++bits) {
    size_t period = size_t(1) << bits;
    if (values.size() < 2 * period)
      continue;
    bool matches = true;
    for (size_t i = period; i < values.size(); ++i)
      if (values[i] != values[i % period]) {
        matches = false;
        break;
      }
    if (matches) {
      return [&t, bits, pattern = std::vector<Term>(values.begin(), values.begin() + period)](
                 const Term &j) {
        Term k = t.extract(j, bits - 1, 0), r = pattern[0];
        for (size_t i = 1; i < pattern.size(); ++i)
          r = t.ite(t.eq(k, t.bv(i, bits)), pattern[i], r);
        return r;
      };
    }
  }
  // Non-power-of-two periods need unsigned remainder; reserve it for large,
  // repeated tables and keep its operand no wider than the table index needs.
  if (values.size() >= 128) {
    unsigned indexBits = std::bit_width(values.size() - 1);
    for (size_t period = 3; period <= 16 && 2 * period <= values.size(); ++period) {
      if ((period & (period - 1)) == 0)
        continue;
      bool matches = true;
      for (size_t i = period; i < values.size(); ++i)
        if (values[i] != values[i % period]) {
          matches = false;
          break;
        }
      if (!matches)
        continue;
      return [&t, period, indexBits,
              pattern = std::vector<Term>(values.begin(), values.begin() + period)](
                 const Term &j) {
        Term index = resized(t, j, indexBits);
        Term k = t.urem(index, t.bv(APInt(indexBits, period)));
        Term r = pattern[0];
        for (size_t i = 1; i < pattern.size(); ++i)
          r = t.ite(t.eq(k, t.bv(i, indexBits)), pattern[i], r);
        return r;
      };
    }
  }
  return {};
}

Terms::Def quadratic(Terms &t, std::span<const Term> values) {
  // Nonlinear arithmetic is not worthwhile for short tables.
  if (values.size() < 256)
    return {};
  auto d1 = difference(t, values[1], values[0]), next = difference(t, values[2], values[1]);
  if (!d1 || !next)
    return {};
  APInt d2 = *next - *d1, expected(d1->getBitWidth(), 0), delta = *d1;
  if (d2.isZero())
    return {};
  for (const Term &v : values) {
    auto actual = difference(t, v, values[0]);
    if (!actual || *actual != expected)
      return {};
    expected += delta;
    delta += d2;
  }
  return [&t, first = values[0], linear = t.bv(*d1), second = t.bv(d2)](const Term &j) {
    unsigned w = first.width();
    // Retain w+1 index and product bits before the exact division by two.
    Term k = resized(t, j, w + 1);
    Term product = t.mul(k, t.sub(k, t.bv(1, w + 1)));
    Term triangle = t.extract(product, w, 1);
    return t.add(first, t.add(t.mul(linear, resized(t, j, w)), t.mul(second, triangle)));
  };
}

Terms::Def piecewise(Terms &t, std::span<const Term> values) {
  if (values.size() < 64)
    return {};
  std::vector<Line> lines;
  for (size_t lo = 0; lo < values.size();) {
    if (lines.size() == MaxSegments || values.size() - lo < MinSegment)
      return {};
    auto step = difference(t, values[lo + 1], values[lo]);
    if (!step)
      return {};
    size_t hi = lo;
    APInt expected(step->getBitWidth(), 0);
    while (hi < values.size()) {
      auto actual = difference(t, values[hi], values[lo]);
      if (!actual || *actual != expected)
        break;
      ++hi;
      expected += *step;
    }
    if (hi - lo < MinSegment)
      return {};
    lines.push_back({values[lo], t.bv(*step), lo, hi});
    lo = hi;
  }
  return [&t, lines = std::move(lines)](const Term &j) {
    Term r = lines.back().at(t, j);
    for (size_t i = lines.size() - 1; i > 0; --i)
      r = t.ite(t.ult(j, t.bv(APInt(j.width(), lines[i - 1].hi))),
                lines[i - 1].at(t, j), r);
    return r;
  };
}

} // namespace

Terms::Def tableFunction(Terms &t, std::span<const Term> values) {
  if (values.size() < 8)
    return {};
  if (auto f = affine(t, values)) return f;
  if (auto f = periodic(t, values)) return f;
  if (auto f = quadratic(t, values)) return f;
  return piecewise(t, values);
}

} // namespace vereda::memory
