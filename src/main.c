/*
 * Copyright 2012 Yuichi Araki. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "forsh.h"

static void start_interpreter()
{
	static size_t const BUFFER_SIZE = 1024;
	char *buffer;
	char *tofree;
	char *token;
	Context *context;
	context = context_new();
	while (TRUE) {
		tofree = buffer = (char *) malloc(BUFFER_SIZE);
		if (NULL == buffer) { 
			break;
		}
		if (NULL == fgets(buffer, BUFFER_SIZE, stdin)) {  /* eof */
			free(tofree);
			break;
		}
		while (NULL != (token = strsep((char **) &buffer, " \n"))) {
			if ('\0' == *token) {
				break;
			}
			context_interpret(context, token);
		}
		context_describe(context);  /* debug */
		free(tofree);
	}
	context_free(context);
}

int main(int argc, char **argv)
{
	start_interpreter();
	return 0;
}
