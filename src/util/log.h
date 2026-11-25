// Stage util: information, progress and warning lines, and stage timings.
#pragma once
#include <llvm/ADT/Twine.h>
#include <string>
#include <llvm/Support/Format.h>
#include <llvm/Support/WithColor.h>
#include <llvm/Support/raw_ostream.h>
#include <chrono>
#include <sys/resource.h>

namespace vereda::util {

inline llvm::raw_ostream &info() {
  llvm::WithColor(llvm::outs(), llvm::raw_ostream::CYAN) << "[INFO]";
  return llvm::outs() << " ";
}

inline void progress(const llvm::Twine &msg) {
  llvm::WithColor(llvm::outs(), llvm::raw_ostream::CYAN) << "[PROGRESS]";
  llvm::outs() << " " << msg << "\n";
}
inline void warning(const llvm::Twine &msg) {
  llvm::WithColor(llvm::outs(), llvm::raw_ostream::YELLOW, /*Bold=*/true) << "[WARNING]";
  llvm::outs() << " " << msg << "\n";
}

class Stopwatch {
public:
  Stopwatch() : start(std::chrono::steady_clock::now()) {}
  // Seconds since construction, formatted like "0.012s".
  llvm::format_object<double> elapsed() const {
    std::chrono::duration<double> d = std::chrono::steady_clock::now() - start;
    return llvm::format("%.3fs", d.count());
  }

private:
  std::chrono::steady_clock::time_point start;
};

// Peak resident memory of the process so far, formatted like "123 MB".
inline std::string peakMemory() {
  rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  return std::to_string(usage.ru_maxrss / 1024) + " MB"; // ru_maxrss is in KB on Linux
}

} // namespace vereda::util
