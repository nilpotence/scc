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

void automata_init (Automata *a, TokenType type) {
	a->type = type;
	a->wordLength = 0;
	a->state = 0;
	a->isFinal = false;
}

void automata_print (Automata *a) {
	char name[WORD_MAX_LENGTH];
	switch (a->type) {
		case TYPE:
			strcpy (name, "TYPE");
			break ;
		case KEYWORD:
			strcpy (name, "KEYBOARD");
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
		case EQ:
			strcpy (name, "EQ");
			break ;
		case NEQ:
			strcpy (name, "NEQ");
			break ;
		case GT:
			strcpy (name, "GT");
			break ;
		case GTE:
			strcpy (name, "GTE");
			break ;
		case LT:
			strcpy (name, "LT");
			break ;
		case LTE:
			strcpy (name, "LTE");
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


int main (int argc, char * argv[]) {
	Automata numberAutomata;

	automata_init (&numberAutomata, NUMBER);

	automata_number_feed (&numberAutomata, '0');
	automata_number_feed (&numberAutomata, '.');
	automata_number_feed (&numberAutomata, '1');
	automata_number_feed (&numberAutomata, '0');

	automata_print (&numberAutomata);
}
