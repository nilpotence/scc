#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

FILE * openFile (char filename[]) {
	FILE *fp;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf (stderr, "Cannot read input file %s !\n", filename);
		exit(EXIT_FAILURE);
	}
	
	return fp;
}


void parseFile (FILE *f, Tokenizer *t) {
	char *line = NULL;
	size_t len = 0;
	ssize_t readBytes;

	while ((readBytes = getline(&line, &len, f)) != -1) {
		tokenizer_feed (t, line, readBytes);
	}

	free(line); 
}

void prettyPrint (Tokenizer *t) {
	Token *token = t->tokens;

	int color = 255;
	while (token != NULL) {
		switch (token->type) {
			case TOKEN_COMMENT: color = 240; break ;
			case TOKEN_TYPE: color = 196; break ;
			case TOKEN_IDENTIFIER: color = 63; break ;
			case TOKEN_NUMBER:
			case TOKEN_BOOLEAN:
			case TOKEN_STRING:
				color = 40; break ;
			default: color = 255;
		}

		printf ("\e[38;5;%im%s\e[0m", color, token->word);

		token = token->next;
	}

	printf ("\n");
}

int main (int argc, char *argv[]) {
	Tokenizer t;


	if (argc < 2) {
		fprintf (stderr, "no input file !");
		exit (EXIT_FAILURE);
	}

	FILE * file = openFile (argv[1]);

	tokenizer_init (&t);

	parseFile (file, &t);

	fclose (file);

	prettyPrint (&t);

	return EXIT_SUCCESS;
}
