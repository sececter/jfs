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
#ifndef JFS_CORE_SOLVER_OPTIONS_H
#define JFS_CORE_SOLVER_OPTIONS_H
#include "llvm/Support/Casting.h"
#include <stdint.h>

namespace jfs {
namespace core {

// TODO: Implement LLVM style RTTI so we
// can have subclasses with solver specific
// options.
class SolverOptions {
  // START: LLVM RTTI boilerplate code
public:
  enum SolverOptionKind { SOLVER_OPTIONS_KIND, CXX_FUZZING_SOLVER_KIND };

private:
  const SolverOptionKind kind;

protected:
  SolverOptions(SolverOptionKind kind) : kind(kind) {}

public:
  SolverOptions() : SolverOptions(SOLVER_OPTIONS_KIND) {}
  virtual ~SolverOptions() {}
  SolverOptionKind getKind() const { return kind; }
  static bool classof(const SolverOptions* so) {
    return so->getKind() == SOLVER_OPTIONS_KIND;
  }
  // END: LLVM RTTI boilerplate code
  // Options common to all solvers
  // FIXME: Timeout should be removed here as the cancel() method exists
  uint64_t maxTime = 0;
};
}
}

#endif
