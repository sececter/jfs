//===----------------------------------------------------------------------===//
//
//                        JFS - The JIT Fuzzing Solver
//
// Copyright 2017 Daniel Liew
//
// This file is distributed under the MIT license.
// See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//
#include "jfs/CXXFuzzingBackend/CXXFuzzingSolverOptions.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
namespace jfs {
namespace cxxfb {

ClangOptions::ClangOptions()
    : pathToBinary(""), pathToRuntimeIncludeDir(""), pathToLibFuzzerLib(""),
      optimizationLevel("0"), useASan(false), useUBSan(false) {}

ClangOptions::ClangOptions(llvm::StringRef pathToExecutable,
                           LibFuzzerBuildType lfbt)
    : ClangOptions() {
  // Try to infer paths
  assert(pathToExecutable.data() != nullptr);
  assert(pathToExecutable.size() > 0);
  assert(llvm::sys::path::is_absolute(llvm::Twine(pathToExecutable)));
  llvm::SmallVector<char, 256> mutablePath(pathToExecutable.begin(),
                                           pathToExecutable.end());
  // Remove "/bin/jfs"
  llvm::sys::path::remove_filename(mutablePath);
  llvm::sys::path::remove_filename(mutablePath);

  // Set path to Clang
  llvm::sys::path::append(mutablePath, "runtime", "bin", "clang++");
  pathToBinary = std::string(mutablePath.data(), mutablePath.size());
  // Remove "bin/clang++"
  llvm::sys::path::remove_filename(mutablePath);
  llvm::sys::path::remove_filename(mutablePath);

  // Set path to runtime include directory
  llvm::sys::path::append(mutablePath, "include");
  pathToRuntimeIncludeDir = std::string(mutablePath.data(), mutablePath.size());
  // Remove "include"
  llvm::sys::path::remove_filename(mutablePath);

  // Set path to libFuzzer library
  switch (lfbt) {
  case LibFuzzerBuildType::REL_WITH_DEB_INFO:
    llvm::sys::path::append(mutablePath, "LibFuzzer_RelWithDebInfo");
    break;
  default:
    llvm_unreachable("Unhandled LibFuzzer build type");
  }
  // FIXME: This is linux specific
  llvm::sys::path::append(mutablePath, "Fuzzer", "libLLVMFuzzer.a");
  pathToLibFuzzerLib = std::string(mutablePath.data(), mutablePath.size());
}

void ClangOptions::appendSanitizerCoverageOption(SanitizerCoverageTy opt) {
  sanitizerCoverageOptions.push_back(opt);
}

void ClangOptions::dump() const { print(llvm::errs()); }

void ClangOptions::print(llvm::raw_ostream& os) const {
  os << "pathToBinary: \"" << pathToBinary << "\"\n";
  os << "pathToRuntimeIncludeDir: \"" << pathToRuntimeIncludeDir << "\"\n";
  os << "pathToLibFuzzerLib: \"" << pathToLibFuzzerLib << "\"\n";
  os << "optimizationLevel: " << optimizationLevel << "\n";
  os << "useASan: " << (useASan ? "true" : "false") << "\n";
  os << "useUBSan: " << (useUBSan ? "true" : "false") << "\n";
  os << "sanitizerCoverageOptions:";
  for (const auto& opt : sanitizerCoverageOptions) {
    switch (opt) {
    case SanitizerCoverageTy::TRACE_PC_GUARD:
      os << " TRACE_PC_GUARD";
      break;
    case SanitizerCoverageTy::TRACE_CMP:
      os << " TRACE_CMP";
      break;
    default:
      llvm_unreachable("Unhandled sanitizer coverage option");
    }
  }
  os << "\n";
}

CXXFuzzingSolverOptions::CXXFuzzingSolverOptions(
    std::unique_ptr<ClangOptions> clangOpt)
    : jfs::core::SolverOptions(CXX_FUZZING_SOLVER_KIND),
      clangOpt(std::move(clangOpt)) {}
}
}