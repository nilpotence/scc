
program -> statement
program -> statement program

statement -> expression ;

statement -> declaration ;
declaration -> TOKEN_TYPE variable TOKEN_ASSIGN expression

statement -> assignation ;
assignation -> variable TOKEN_ASSIGN expression

statement -> if
statement -> if else
statement -> if elseif
statement -> if elseif else
elseif -> TOKEN_KEYWORD(else if) TOKEN_OPEN_PAR expression TOKEN_CLOSE_PAR block
if -> TOKEN_KEYWORD(if) TOKEN_OPEN_PAR expression TOKEN_CLOSE_PAR block
else -> TOKEN_KEYWORD(else) block

statement -> while
while -> TOKEN_KEYWORD(while) TOKEN_OPEN_PAR expression TOKEN_CLOSE_PAR block

block -> TOKEN_OPEN_CURLY program TOKEN_CLOSE_CURLY


expression -> expression
expression -> TOKEN_CLOSE_PAR expression TOKEN_CLOSE_PAR
expression -> expression TOKEN_OPERATOR expression
expression -> TOKEN_BOOLEAN | TOKEN_NUMBER | TOKEN_STRING | variable

variable -> TOKEN_IDENTIFIER
