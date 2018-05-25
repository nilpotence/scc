#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define WORD_MAX_LENGTH 255


typedef enum {
	EMPTY = 0,
	INTERNAL,
	K_INT,
	K_FLOAT,
	K_IF,
	K_EQ,
	K_ADD,
	K_SUB,
	K_SEMICOLON,
	K_DOT,
	K_OPEN_PAR,
	K_CLOSE_PAR,
	K_SPACE,
	K_LINE_RETURN,
	INTEGER,
	FLOAT,
	IDENTIFIER
} TokenType;


struct _State;

typedef struct _StateTransition {
	char event;
	struct _State * newState;
	struct _StateTransition * next;
} StateTransition;

typedef struct _State {
	TokenType type;
	StateTransition *transitions;
} State;


typedef struct _Tokenizer {
	State * startState;
	State * currentState;
	char currentWord[WORD_MAX_LENGTH];
} Tokenizer;


State * state_new (TokenType type) {
	State * state = (State*) malloc (sizeof (State));
	state->type = type;
	state->transitions = NULL;
	return state;
}


Tokenizer * tokenizer_new () {
	Tokenizer *t = (Tokenizer*) malloc (sizeof (Tokenizer));
	t->startState = state_new (EMPTY);
	t->currentState = t->startState;
	strncpy (t->currentWord, "", WORD_MAX_LENGTH);
	return t;
}

void tokenizer_reset (Tokenizer * t) {
	t->currentState = t->startState;
}

void tokenizer_add_keyword (Tokenizer *t, TokenType type, const char keyword[]) {
	int keywordLength = strlen (keyword);

	for (int i = 0; i < keywordLength; i++) {
		StateTransition *trans = t->currentState->transitions;
		StateTransition *prevTrans = NULL;

		while (trans != NULL && trans->event != keyword[i]) {
			trans = trans->next;
		}

		
	}
}


int main (int argc, char * argv[]) {

	Tokenizer * t = tokenizer_new ();

	tokenizer_add_keyword (K_INT, "int");

	return 0;
}
