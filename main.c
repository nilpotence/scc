#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "tokens.h"

void automata_do_feed (Automata *a, char c) {
	if (a->wordLength >= WORD_MAX_LENGTH - 1) {
		fprintf (stderr, "WORD_MAX_LENGTH reached  (token = %s)! \n", a->word);
		exit (1);
	}

	a->word[a->wordLength] = c;
	a->word[a->wordLength + 1] = '\0';
	a->wordLength ++;
}

void automata_reset (Automata *a) {
	a->state = 0;
	a->isFinal = false;
	strcpy (a->word, "");
	a->wordLength = 0;
}

void automata_print (Automata *a) {
	char name[WORD_MAX_LENGTH];
	switch (a->type) {
		case TYPE:
			strcpy (name, "TYPE");
			break ;
		case KEYWORD:
			strcpy (name, "KEYWORD");
			break ;
		case OPERATOR:
			strcpy (name, "OPERATOR");
			break ;
		case SEPARATOR:
			strcpy (name, "SEPARATOR");
			break ;
		case OPEN_PAR:
			strcpy (name, "OPEN_PAR");
			break ;
		case CLOSE_PAR:
			strcpy (name, "CLOSE_PAR");
			break ;
		case ASSIGN:
			strcpy (name, "ASSIGN");
			break ;
		case NUMBER:
			strcpy (name, "NUMBER");
			break ;
		case BOOLEAN:
			strcpy (name, "BOOLEAN");
			break ;
		case COMMENT:
			strcpy (name, "COMMENT");
			break ;
		case STRING:
			strcpy (name, "STRING");
			break ;
		case IDENTIFIER:
			strcpy (name, "IDENTIFIER");
			break ;
	}

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


int automata_words_feed (Automata *a, const char * words[], int wordsCount, char c) {
	int hasMatches = true;

	if (a->state == -1) {
		return false;
	}

	for (int i = 0; i < wordsCount; i++) {
		for (int j = 0; j < a->wordLength; j ++) {
			if (words[i][j] == '\0' || words[i][j] != a->word[j]) {
				hasMatches = false;
				break ;
			}

			if (hasMatches == true) {
				break ;
			} else {
				hasMatches = true;
			}
		}
	}

	if (hasMatches == true) {
		automata_do_feed (a, c);
	} else {
		a->state = -1;
	}

	return hasMatches;
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


#define AUTOMATA_COUNT 12

static const Automata numberAutomata[AUTOMATA_COUNT] = {
	{ TYPE, automata_type_feed, 0, false, "", 0},
	{ KEYWORD, automata_keyword_feed, 0, false, "", 0},
	{ OPERATOR, automata_operator_feed, 0, false, "", 0},
	{ SEPARATOR, automata_separator_feed, 0, false, "", 0},
	{ OPEN_PAR, automata_open_par_feed, 0, false, "", 0},
	{ CLOSE_PAR, automata_close_par_feed, 0, false, "", 0},
	{ ASSIGN, automata_assign_feed, 0, false, "", 0},
	{ NUMBER, automata_number_feed, 0, false, "", 0},
	{ STRING, automata_string_feed, 0, false, "", 0},
	{ COMMENT, automata_comment_feed, 0, false, "", 0},
	{ BOOLEAN, automata_boolean_feed, 0, false, "", 0},
	{ IDENTIFIER, automata_identifier_feed, 0, false, "", 0}
};






