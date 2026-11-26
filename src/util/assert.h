// Stage util: names and arities of libc assertion-failure entry points.
#pragma once
#include <string_view>

namespace vereda {

inline bool isAssertFail(std::string_view name, unsigned args) {
  if (name == "__assert")
    return args == 3 || args == 4;
  if (name == "_wassert")
    return args == 3;
  return args == 4 && (name == "__assert_fail" || name == "__assert_rtn" ||
                       name == "__assert_func" || name == "__assert_perror_fail");
}

} // namespace vereda
