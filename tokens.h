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
	EQ,
	NEQ,
	GT,
	GTE,
	LT,
	LTE,
	ASSIGN,
	NUMBER,
	BOOLEAN,
	IDENTIFIER
} TokenType;

typedef struct {
	TokenType type;
	int state;
	int isFinal;
	char word[WORD_MAX_LENGTH];
	char wordLength;
} Automata;

#endif
