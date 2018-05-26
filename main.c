#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define WORD_MAX_LENGTH 255
#define TRUE 1
#define FALSE 0

struct _State;

typedef struct _StateTransition {
	char event[WORD_MAX_LENGTH];
	struct _State * state;
	struct _StateTransition * next;
} StateTransition;

typedef struct _State {
	char stateName[WORD_MAX_LENGTH];
	int isFinal;
	StateTransition *transitions;
} State;


typedef struct _Automata {
	State * startState;
	State * currentState;
	char currentWord[WORD_MAX_LENGTH];
	int currentWordLen;
} Automata;

int min (int a, int b) {
	return a < b ? a : b;
}

int max (int a, int b) {
	return a > b ? a : b;
}

State * state_new (const char * stateName, int stateNameLen, int isFinal) {
	State * state = (State*) malloc (sizeof (State));
	strncpy (state->stateName, stateName, stateNameLen);
	state->isFinal = isFinal;
	state->transitions = NULL;
	return state;
}

void state_transition_add (State * state, StateTransition *transition) {
	if (state->transitions == NULL) {
		state->transitions = transition;
		return ;
	}

	StateTransition *trans = state->transitions;
	while (trans->next != NULL) trans = trans->next;

	trans->next = transition;
}

StateTransition * state_transition_new (const char event[], State * newState) {
	StateTransition * transition = (StateTransition *) malloc (sizeof (StateTransition));
	strcpy(transition->event, event);
	transition->state = newState;
	transition->next = NULL;
	return transition;
}

int state_transition_is_applicable (StateTransition * transition, const char event) {
	int eventLen = strlen (transition->event);
	for (int i = 0; i < eventLen; i++) {
		if (transition->event[i] == event) {
			return TRUE;
		}
	}
	return FALSE;
}

Automata * automata_new () {
	Automata *t = (Automata*) malloc (sizeof (Automata));
	t->startState = state_new ("", 0, FALSE);
	t->currentState = t->startState;
	strncpy (t->currentWord, "", WORD_MAX_LENGTH);
	t->currentWordLen = 0;
	return t;
}

void automata_reset (Automata * t) {
	t->currentState = t->startState;
	t->currentWordLen = 0;
}


int automata_feed_char (Automata * t, char c) {
	if (t->currentState->transitions == NULL) {
		return FALSE;
	}

	StateTransition * trans = t->currentState->transitions;
	while (trans != NULL && !state_transition_is_applicable (trans, c)) {
		trans = trans->next;
	}

	if (trans == NULL) {
		return FALSE;
	}

	t->currentState = trans->state;
	t->currentWord[t->currentWordLen] = c;
	t->currentWordLen ++;
	
	return TRUE;
}

int automata_is_valid (Automata *t, const char word[]) {
	int len = strlen (word);
	for (int i = 0; i < len; i++) {
		if (!automata_feed_char (t, word[i])) {
			automata_reset (t);
			return FALSE;
		}
	}

	int isValid = t->currentState->isFinal;
	automata_reset (t);
	return isValid;
}

void automata_add_keyword (Automata *t, const char keyword[]) {
	int keywordLength = strlen (keyword);
	char event[WORD_MAX_LENGTH];

	for (int i = 0; i < keywordLength; i++) {
		State *newState = NULL;
		StateTransition *trans = t->currentState->transitions;
		StateTransition *prevTrans = NULL;

		if (trans == NULL) {
			newState = state_new (keyword, i + 1, FALSE);
			strncpy (event, &keyword[i], 1);
			trans = state_transition_new (event, newState);	
			t->currentState->transitions = trans;
		} else {
			while (trans != NULL && ! state_transition_is_applicable (trans, keyword[i])) {
				prevTrans = trans;
				trans = trans->next;
			}

			if (trans == NULL) {
				newState = state_new (keyword, i + 1, FALSE);
				strncpy (event, &keyword[i], 1);
				trans = state_transition_new (event, newState);
				prevTrans->next = trans;
			} 
		}
		t->currentState = trans->state;
	}
	
	t->currentState->isFinal = TRUE;
	automata_reset (t);
}

void automata_state_show (State *s, int depth) {
	if (s->isFinal) {
		printf ("[[%s]]\n", s->stateName);
	} else {
		printf ("[%s]\n", s->stateName);
	}
	
	StateTransition *trans = s->transitions;
	while (trans != NULL) {
		for (int i = 0; i < depth; i++) putchar ('\t');
		printf ("|---> ");
		automata_state_show (trans->state, depth + 1);
		trans = trans->next;
	}
}

void automata_show (Automata *t) {
	automata_state_show (t->startState, 0);
}

Automata * number_automata_new () {

	State *initState = state_new ("", 0, FALSE);
	State *integerState = state_new ("INTEGER", 7, TRUE);
	State *floatState = state_new ("FLOAT", 5, TRUE);

	StateTransition *integerTransition = state_transition_new ("0123456789", integerState);
	state_transition_add (initState, integerTransition);
	state_transition_add (integerState, integerTransition);

	StateTransition *toFloatTransition = state_transition_new (".", floatState);
	state_transition_add (integerState, toFloatTransition);
	StateTransition *floatTransition = state_transition_new ("0123456789", floatState);
	state_transition_add (floatState, floatTransition);

	Automata *automata = automata_new ();
	automata->startState = initState;
	automata->currentState = initState;

	return automata;
}

Automata * identifier_automata_new () {
	State *initState = state_new ("", 0, FALSE);
	State *idState = state_new ("IDENTIFIER", 10, TRUE);
	StateTransition * idTransition = state_transition_new ("abcdefghijklmnopkrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_", idState);
	state_transition_add (initState, idTransition);
	state_transition_add (idState, idTransition);

	Automata *automata = automata_new ();
	automata->startState = initState;
	automata->currentState = initState;

	return automata;
}


Automata * space_automata_new () {
	State *initState = state_new ("", 0, FALSE);
	State *spaceState = state_new ("IDENTIFIER", 10, TRUE);
	StateTransition * spaceTransition = state_transition_new (" \t\n", spaceState);
	state_transition_add (initState, spaceTransition);
	state_transition_add (spaceState, spaceTransition);

	Automata *automata = automata_new ();
	automata->startState = initState;
	automata->currentState = initState;

	return automata;
}

int main (int argc, char * argv[]) {

	Automata * keywordAutomata = automata_new ();

	automata_add_keyword (keywordAutomata, "int");
	automata_add_keyword (keywordAutomata, "if");
	automata_add_keyword (keywordAutomata, "float");
	automata_add_keyword (keywordAutomata, "+");
	automata_add_keyword (keywordAutomata, "-");
	automata_add_keyword (keywordAutomata, ";");
	automata_add_keyword (keywordAutomata, "->");
	automata_add_keyword (keywordAutomata, "(");
	automata_add_keyword (keywordAutomata, ")");
	automata_add_keyword (keywordAutomata, "!=");
	automata_add_keyword (keywordAutomata, "==");
	automata_add_keyword (keywordAutomata, "<");
	automata_add_keyword (keywordAutomata, ">");
	automata_add_keyword (keywordAutomata, "=>");
	automata_add_keyword (keywordAutomata, ">=");
	automata_add_keyword (keywordAutomata, "=<");
	automata_add_keyword (keywordAutomata, "<=");
	automata_add_keyword (keywordAutomata, "=");

	Automata * numberAutomata = number_automata_new ();
	Automata *identifierAutomata = identifier_automata_new ();
	Automata *spaceAutomata = space_automata_new ();

	Automata * automatas[4] = { keywordAutomata, numberAutomata, identifierAutomata, spaceAutomata };
	Automata * valids[4] = { keywordAutomata, numberAutomata, identifierAutomata, spaceAutomata };
	Automata * invalids[4] = { NULL, NULL, NULL, NULL };

	const char program[] = "int a = 1;\n int b = 4;  int c = a + b; ";
	int programLen = strlen (program);

	for (int i = 0; i < programLen; i++) {
		int hasValids = FALSE;	
		for (int j = 0; j < 4; j++) {
			if (valids[j] == NULL) continue ;
			if (!automata_feed_char(valids[j], program[i])) {
				invalids[j] = valids[j];
				valids[j] = NULL;
			} else {
				hasValids = TRUE;
			}
		}

		if (hasValids) {
			for (int j = 0; j < 4; j++) invalids[j] = NULL;
			continue ;
		}

		int selectedAutomata = -1;
		for (int j = 0; j < 4; j++) {
			if (invalids[j] == NULL) continue ;
			if (invalids[j]->currentState->isFinal) {
				selectedAutomata = j;
				break ;
			}
		}

		char word[WORD_MAX_LENGTH] = "";

		int m = max (0, i - 10);
		int M = min (programLen, i + 10);		
		strncpy (word, &program[m], M - m);
		word[M] = 0;
		if (selectedAutomata == -1) {
			printf ("Syntax error :\n");
			printf ("%s\n\n", word);
			break ;
		}

		strncpy (word, invalids[selectedAutomata]->currentWord, invalids[selectedAutomata]->currentWordLen);
		word[invalids[selectedAutomata]->currentWordLen] = 0;
		printf ("TOKEN : %s\n", word);

		for (int j = 0; j < 4; j++) {
			invalids[j] = NULL;
			valids[j] = automatas[j];
			automata_reset (automatas[j]);
		}
		i = i - 1;
	}
	


	return 0;
}
