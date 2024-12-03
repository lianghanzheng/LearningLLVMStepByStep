#ifndef TOKEN
#define TOKEN(type, spelling)
#endif

TOKEN(kw_int,     "int")
TOKEN(kw_if,      "if")
TOKEN(kw_else,    "else")

TOKEN(plus,       "+")
TOKEN(minus,      "-")
TOKEN(star,       "*")
TOKEN(slash,      "/")
TOKEN(lparen,     "(")
TOKEN(rparen,     ")")
TOKEN(lbrace,     "{")
TOKEN(rbrace,     "}")
TOKEN(comma,      ",")
TOKEN(semi,       ";")
TOKEN(equal,      "=")
TOKEN(equalequal, "==")
TOKEN(notequal,   "!=")
TOKEN(less,       "<")
TOKEN(lesseq,     "<=")
TOKEN(greater,    ">")
TOKEN(greatereq,  ">=")
TOKEN(identifier, "identifier")
TOKEN(number,     "number")

#undef TOKEN