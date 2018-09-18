#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"


int main (int argc, char *argv[]) {
	Tokenizer t;
	char program[] = "number a = 1; ";
	int programLength = strlen(program);

	tokenizer_init (&t);
	tokenizer_feed (&t, program, programLength);

	return 0;
}
