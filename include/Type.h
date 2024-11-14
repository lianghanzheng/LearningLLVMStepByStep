#ifndef TYPE_H_
#define TYPE_H_

#include <cstddef>

enum class TypeKind {
  Int,
};

class CType {
public:
  CType(TypeKind tk, size_t size, size_t align)
      : kind(tk), size(size), align(align) {}
  // Singleton pattern
  static CType *getIntTy();
private:
  size_t size;
  size_t align;
  TypeKind kind;
};

#endif // TYPE_H_