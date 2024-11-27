#include "DiagEngine.h"

static const char *diagMsg[] = {
# define DIAG(ID, KIND, MSG) MSG,
# include "Diag.h.inc"
};

static const llvm::SourceMgr::DiagKind diagKind[] = {
# define DIAG(ID, KIND, MSG) llvm::SourceMgr::DK_##KIND,
# include "Diag.h.inc"
};

const char *DiagEngine::getDiagMessage(unsigned id) {
  return diagMsg[id];
}

llvm::SourceMgr::DiagKind DiagEngine::getDiagKind(unsigned id) {
  return diagKind[id];
}
