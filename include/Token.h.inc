#ifndef TOKEN
#define TOKEN(type, spelling)
#endif

TOKEN(kw_int,     "int")

TOKEN(plus,       "+")
TOKEN(minus,      "-")
TOKEN(star,       "*")
TOKEN(slash,      "/")
TOKEN(lparen,     "(")
TOKEN(rparen,     ")")
TOKEN(semi,       ";")
TOKEN(equal,      "=")
TOKEN(comma,      ",")
TOKEN(identifier, "identifier")
TOKEN(number,     "number")

#undef TOKEN