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
	Context *context;
	Error *error;
	context = context_new();
	while (TRUE) {
		char *tofree;
		char *token;
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
			error = context_interpret(context, token);
			if (NULL != error) {
				char buf[1024];
				fprintf(stderr, "%s\n", error_str(error, buf, sizeof(buf)));
				error_free(error);
			}
		}
		context_describe(context);  /* debug */
		free(tofree);
	}
	context_free(context);
}

int main()
{
	start_interpreter();
	return 0;
}
