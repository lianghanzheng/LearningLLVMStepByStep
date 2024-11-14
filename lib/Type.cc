#include "Type.h"

CType *CType::getIntTy() {
  static CType ctype(TypeKind::Int, 4, 4);
  return &ctype;
}