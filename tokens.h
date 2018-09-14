#ifndef _TOKENS_H
#define _TOKENS_H


#define WORD_MAX_LENGTH 255



typedef enum {
	TYPE,
	KEYWORD,
	OPERATOR,
	SEPARATOR,
	OPEN_PAR,
	CLOSE_PAR,
	ASSIGN,
	NUMBER,
	STRING,
	COMMENT,
	BOOLEAN,
	IDENTIFIER
} TokenType;

typedef struct _Automata {
	TokenType type;
	int (*feedFunc)(struct _Automata *, char);
	int state;
	int isFinal;
	char word[WORD_MAX_LENGTH];
	char wordLength;
} Automata;


#endif
