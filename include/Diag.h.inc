#ifndef DIAG
#define DIAG(ID, KIND, MSG)
#endif

// Lexer
DIAG(err_unknown_char, Error, "unknown char '{0}'")

// Parser
DIAG(err_expected_token, Error, "expected '{0}', but get '{1}'")
DIAG(err_break_stmt, Error, "'break' statement not in loop or switch statement")
DIAG(err_continue_stmt, Error, "'continue' statement not in loop or switch statement")

// Sema
DIAG(err_redefined, Error, "Symbol '{0}' has been defined")
DIAG(err_undefined, Error, "Symbol '{0}' is not defined")
DIAG(err_lvalue, Error, "Lvalue required for the left-hand side of assign expression")

#undef DIAG