#ifndef DIAGENGINE_H_
#define DIAGENGINE_H_

#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/SourceMgr.h"
#include <utility>

namespace diag {
enum {
# define DIAG(ID, KIND, MSG) ID,
# include "Diag.h.inc"
};
} // namespace diag

class DiagEngine {
public:
  DiagEngine(llvm::SourceMgr &mgr) : mgr(mgr) {}

  template<typename... Args>
  void report(llvm::SMLoc loc, unsigned diagID, Args... args) {
    auto diagKind = getDiagKind(diagID);
    auto diagMsgFmt = getDiagMessage(diagID);

    mgr.PrintMessage(loc, diagKind, llvm::formatv(diagMsgFmt, std::forward<Args>(args)...));
    
    if (diagKind == llvm::SourceMgr::DK_Error) {
      exit(0);
    }
  }
private:
  llvm::SourceMgr::DiagKind getDiagKind(unsigned id);
  const char *getDiagMessage(unsigned id);

private:
  llvm::SourceMgr &mgr;
};

#endif // DIAGENGINE_H_