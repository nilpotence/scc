#ifndef _TOKENS_H
#define _TOKENS_H


#define WORD_MAX_LENGTH 255
#define AUTOMATA_COUNT 13



typedef enum {
	TOKEN_TYPE = 0,
	TOKEN_SPACE,
	TOKEN_KEYWORD,
	TOKEN_OPERATOR,
	TOKEN_SEPARATOR,
	TOKEN_OPEN_PAR,
	TOKEN_CLOSE_PAR,
	TOKEN_ASSIGN,
	TOKEN_NUMBER,
	TOKEN_STRING,
	TOKEN_COMMENT,
	TOKEN_BOOLEAN,
	TOKEN_IDENTIFIER
} TokenType;

typedef struct _Automata {
	TokenType type;
	int (*feedFunc)(struct _Automata *, char);
	int state;
	int isFinal;
	char word[WORD_MAX_LENGTH];
	char wordLength;
} Automata;


typedef struct _Token {
	TokenType type;
	char word[WORD_MAX_LENGTH];
	char wordLength;

	struct _Token * prev;
	struct _Token * next;
} Token;

typedef struct {
	Token token;
	double value;
} NumberToken;

typedef struct {
	Token token;
	char * value;
	int length;
} StringToken;

typedef enum {
	TYPE_NUMBER = 0,
	TYPE_STRING,
	TYPE_BOOLEAN
} TypeValue;

typedef struct {
	Token token;
	TypeValue value;
} TypeToken;

typedef enum {
	KEYWORD_IF = 0,
	KEYWORD_ELSE,
	KEYWORD_ELSEIF,
	KEYWORD_WHILE
} KeywordValue;

typedef struct {
	Token token;
	KeywordValue value;
} KeywordToken;

typedef enum {
	OPERATOR_ADD = 0,
	OPERATOR_SUB,
	OPERATOR_LT,
	OPERATOR_LTE,
	OPERATOR_EQ,
	OPERATOR_GTE,
	OPERATOR_GT,
	OPERATOR_DIV,
	OPERATOR_MUL
} OperatorValue;

typedef struct {
	Token token;
	OperatorValue value;
} OperatorToken;

typedef struct {
	Token token;
	int value;
} BooleanToken;

typedef struct _Tokenizer {
	Automata automatas[AUTOMATA_COUNT];
	Automata * valids[AUTOMATA_COUNT];
	Automata * invalids[AUTOMATA_COUNT];

	Token * tokens;
} Tokenizer;


void tokenizer_init (Tokenizer *t);
void tokenizer_feed (Tokenizer *t, char chunk[], int chunkLength);


#endif
