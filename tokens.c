#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "tokens.h"


typedef struct {
	TokenType type;
	char name[WORD_MAX_LENGTH];
	int (*automataFeed)(Automata *, char);
	void (*parse)(Token *);	
	size_t tokenSize;
} TokenDescriptor;



void automata_init (Automata *a, TokenType t);
void automata_reset (Automata *a);
void automata_print (Automata *a);


int automata_type_feed (Automata * a, char c);
int automata_space_feed (Automata *a, char c);
int automata_keyword_feed (Automata *a, char c);
int automata_operator_feed (Automata *a, char c);
int automata_separator_feed (Automata *a, char c);
int automata_open_par_feed (Automata *a, char c);
int automata_close_par_feed (Automata *a, char c);
int automata_assign_feed (Automata *a, char c);
int automata_number_feed (Automata *a, char c);
int automata_boolean_feed (Automata *a, char c);
int automata_comment_feed (Automata *a, char c);
int automata_string_feed (Automata *a, char c);
int automata_identifier_feed (Automata *a, char c);


//declaration order must be the same as enum value's declaration order (enum values are used for indexing)
static const TokenDescriptor tokenDescriptors[AUTOMATA_COUNT] = {
	{ TOKEN_TYPE, 		"TYPE",		automata_type_feed,		NULL, sizeof (TypeToken) },
	{ TOKEN_SPACE, 		"SPACE",	automata_space_feed,		NULL, sizeof (Token) },
	{ TOKEN_KEYWORD,	"KEYWORD",	automata_keyword_feed,		NULL, sizeof (KeywordToken) },
	{ TOKEN_OPERATOR,	"OPERATOR",	automata_operator_feed,		NULL, sizeof (OperatorToken) },
	{ TOKEN_SEPARATOR,	"SEPARATOR",	automata_separator_feed,	NULL, sizeof (Token) },
	{ TOKEN_OPEN_PAR,	"OPEN_PAR",	automata_open_par_feed,		NULL, sizeof (Token) },
	{ TOKEN_CLOSE_PAR,	"CLOSE_PAR",	automata_close_par_feed,	NULL, sizeof (Token) },
	{ TOKEN_ASSIGN,		"ASSIGN",	automata_assign_feed,		NULL, sizeof (Token) },
	{ TOKEN_NUMBER,		"NUMBER",	automata_number_feed,		NULL, sizeof (NumberToken) },
	{ TOKEN_STRING,		"STRING",	automata_string_feed,		NULL, sizeof (StringToken) },
	{ TOKEN_COMMENT,	"COMMENT",	automata_comment_feed,		NULL, sizeof (Token) },
	{ TOKEN_BOOLEAN,	"BOOLEAN",	automata_boolean_feed,		NULL, sizeof (BooleanToken) },
	{ TOKEN_IDENTIFIER,	"IDENTIFIER",	automata_identifier_feed,	NULL, sizeof (Token) }
};

void automata_do_feed (Automata *a, char c) {
	if (a->wordLength >= WORD_MAX_LENGTH - 1) {
		fprintf (stderr, "WORD_MAX_LENGTH reached  (token = %s)! \n", a->word);
		exit (1);
	}

	a->word[a->wordLength] = c;
	a->word[a->wordLength + 1] = '\0';
	a->wordLength ++;
}

void automata_init (Automata *a, TokenType type) {
	a->type = type;
	automata_reset (a);
}

void automata_reset (Automata *a) {
	a->state = 0;
	a->isFinal = false;
	strcpy (a->word, "");
	a->wordLength = 0;
}

void automata_print (Automata *a) {
	char name[WORD_MAX_LENGTH];
	strcpy (name, tokenDescriptors[a->type].name);

	printf ("Type = %s\n", name);
	printf ("State = %i\n", a->state);
	if (a->isFinal) {
		printf ("Final = yes\n");
	} else {
		printf ("Final = no\n");
	}
	printf ("Word Length = %i\n", a->wordLength);
	printf ("Word = %s\n", a->word);
}

int automata_number_feed (Automata *a, char c) {
	switch (a->state) {
		case 0:
			if (c >= '0' && c <= '9') {
				automata_do_feed (a, c);
				a->isFinal = true;
				return true;
			} else if (c == '.') {
				automata_do_feed (a, c);
				a->isFinal = true;
				a->state = 1;
				return true;
			} else {
				a->state = -1;
				return false;
			}
		case 1:
			if (c >= '0' && c <= '9') {
				automata_do_feed (a, c);
				a->isFinal = true;
				break ;
			} else {
				a->state = -1;
				return false;
			}
		case -1:
		default:
			a->state = -1;
			return false;
	}
}

int automata_identifier_feed (Automata *a, char c) {
	switch (a->state) {
		case 0:
			if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_') {
				automata_do_feed (a, c);
				a->isFinal = true;
				return true;
			} else {
				a->state = -1;
				return false;
			}
		case -1:
		default:
			a->state = -1;
			return false;
	}
}

int automata_comment_feed (Automata *a, char c) {
	switch (a->state) {
		case 0:
			if (c == '/') {
				automata_do_feed (a, c);
				a->state = 1;
				return true;
			} else {
				a->state = -1;
				return false;
			}
		case 1:
			if (c == '*') {
				automata_do_feed (a, c);
				a->state = 2;
				return true;
			} else if (c == '/') {
				automata_do_feed (a, c);
				a->state = 3;
				return true;
			} else {
				a->state = -1;
				return false;
			}
		case 2: //multi line comment
			if (c == '*') {
				automata_do_feed (a, c);
				a->state = 4;
				return true;
			} else {
				automata_do_feed (a, c);
				return true;
			}
		case 3: //inline comment
			if (c == '\n') {
				automata_do_feed (a, c);
				a->state = -1;
				a->isFinal = true;
				return true;
			}
		case 4:
			if (c == '/') {
				automata_do_feed (a, c);
				a->state = -1;
				a->isFinal = true;
				return true;
			} else {
				automata_do_feed (a, c);
				a->state = 2;
				return true;
			}
		case -1:
		default:
			a->state = -1;
			return false;

	}

}


int automata_string_feed (Automata *a, char c) {
	switch (a->state) {
		case 0:
			if (c == '"') {
				automata_do_feed (a, c);
				a->state = 1;
				return true;
			} else {
				a->state = -1;
				return false;
			}
		case 1:
			if (c == '"') {
				automata_do_feed (a, c);
				a->state = -1;
				a->isFinal = true;
				return true;
			} else if (c == '\\') {
				automata_do_feed (a, c);
				a->state = 2;
				return true;
			} else {
				automata_do_feed (a, c);
				return true;
			}
		case 2:
			automata_do_feed (a, c);
			a->state = 1;
			return true;
		case -1:
		default:
			a->state = -1;
			return false;
	}
}

int automata_space_feed (Automata *a, char c) {
	switch (a->state) {
		case 0:
			if (c == ' ' || c == '\n' || c == '\t') {
				automata_do_feed (a, c);
				a->isFinal = true;
				return true;
			}
		case -1:
		default:
			a->state = -1;
			return false;
	}
}

int automata_words_feed (Automata *a, const char * words[], int wordsCount, char c) {
	char word[WORD_MAX_LENGTH];

	if (a->state == -1 || a->wordLength + 1 > WORD_MAX_LENGTH) {
		return false;
	}

	strncpy (word, a->word, a->wordLength);
	word[a->wordLength] = c;

	for (int i = 0; i < wordsCount; i++) {
		if (strncmp (word, words[i], a->wordLength + 1) == 0) {
			automata_do_feed (a, c);

			a->isFinal = false;
			if (words[i][a->wordLength] == '\0') {
				a->isFinal = true;
			}

			return true; 
		}
	}
	
	a->state = -1;
	return false;
}

int automata_char_feed (Automata *a, const char ref, char c) {
	switch (a->state) {
		case 0:
			a->state = -1;
			if (c == ref) {
				a->isFinal = true;
				automata_do_feed (a, c);
				return true;
			} else {
				return false;
			}
		case -1:
		default:
			a->state = -1;
			return false;
	}
}

int automata_operator_feed (Automata *a, char c) {
	const char *words[13] = {"+", "-", "/", "*", ">", ">=", "=>", "<", "<=", "=<", "==", "!=", "!"};
	return automata_words_feed (a, words, 13, c);
}

int automata_type_feed (Automata *a, char c) {
	const char *words[3] = {"number", "string", "boolean"};
	return automata_words_feed (a, words, 3, c);
}

int automata_keyword_feed (Automata *a, char c) {
	const char *words[4] = {"if", "else", "else if", "while"};
	return automata_words_feed (a, words, 4, c);
}

int automata_boolean_feed (Automata *a, char c) {
	const char *words[2] = {"true", "false"};
	return automata_words_feed (a, words, 2, c) ;
}

int automata_assign_feed (Automata *a, char c) {
	return automata_char_feed (a, '=', c);
}

int automata_open_par_feed (Automata *a, char c) {
	return automata_char_feed (a, '(', c);
}

int automata_close_par_feed (Automata *a, char c) {
	return automata_char_feed (a, ')', c);
}

int automata_open_curly_feed (Automata *a, char c) {
	return automata_char_feed (a, '{', c);
}

int automata_close_curly_feed (Automata *a, char c) {
	return automata_char_feed (a, '}', c);
}

int automata_separator_feed (Automata *a, char c) {
	return automata_char_feed (a, ';', c);
}

void tokenizer_create_token (Tokenizer *t, Automata *a) {
	Token *token = NULL;
	TokenDescriptor descriptor = tokenDescriptors[a->type];

	token = (Token*) malloc (descriptor.tokenSize);
	if (token == NULL) {
		fprintf (stderr, "cannot allocate memory ! abort !\n");
		exit (1);
	}

	token->type = a->type;
	strncpy (token->word, a->word, a->wordLength);
	token->wordLength = a->wordLength;
	token->next = NULL;

	if (descriptor.parse != NULL) {
		descriptor.parse (token);
	}

	if (t->tokens == NULL) {
		t->tokens = token;
		token->prev = NULL;
	} else {
		Token *tmp = t->tokens;
		while (token->next != NULL) tmp = tmp->next;
		tmp->next = token;
		token->prev = tmp;
	}

	printf ("(%s: %s)\n", descriptor.name, token->word);
}

void tokenizer_init (Tokenizer *t) {
	for (int i = 0; i < AUTOMATA_COUNT; i++) {
		automata_init (&(t->automatas[i]), tokenDescriptors[i].type);
		t->valids[i] = &(t->automatas[i]);
		t->invalids[i] = NULL;
	}

	t->tokens = NULL;
}

void tokenizer_feed_char (Tokenizer *t, char c) {
	int hasValids = false;

	for (int i = 0; i < AUTOMATA_COUNT; i++) {
		if (t->valids[i] == NULL) continue ;
		
		if (! tokenDescriptors[t->valids[i]->type].automataFeed (t->valids[i], c)) {
			t->invalids[i] = t->valids[i];
			t->valids[i] = NULL;
		} else {
			hasValids = true;
		}
	}

	//keep accepting input until all automatas reject input
	if (hasValids) {
		for (int i = 0; i < AUTOMATA_COUNT; i++) {
			t->invalids[i] = NULL;
		}	
		return ;
	}

	int hasFinal = false;

	//search for an automata which is a final state (one which accepts the current input)
	for (int i = 0; i < AUTOMATA_COUNT; i++) {
		if (t->invalids[i] == NULL) continue ;
		
		if (t->invalids[i]->isFinal) {
			tokenizer_create_token (t, t->invalids[i]);
			hasFinal = true;
			break ;
		}
	}

	//reset all automatas to initial state
	for (int i = 0; i < AUTOMATA_COUNT; i++) {
		t->valids[i] = &(t->automatas[i]);
		automata_reset (t->valids[i]);
		t->invalids[i] = NULL;
	}

	if (!hasFinal) {
		//no token found for current input, skip it 
		fprintf (stderr, "\n\nsyntax error !!\n\n");
	} else {
		//re-feed char that caused a token to be emitted
		tokenizer_feed_char (t, c);
	}
}

void tokenizer_feed (Tokenizer *t, char programChunk[], int chunkLength) {
	for (int i = 0; i < chunkLength; i++) {
		tokenizer_feed_char (t, programChunk[i]);
	}
}


int main (int argc, char * argv[]) {
	
	Tokenizer t;
	tokenizer_init (&t);

	char program[] = "number a = 1; ";
	int programLength = strlen(program);

	tokenizer_feed (&t, program, programLength);

	return 0;
}
