/*
 * Copyright 2012 Yuichi Araki. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "forsh.h"

Error *error_new(ErrorType type, char const *message)
{
	Error *error;
	error = (Error *) malloc(sizeof(Error));
	if (NULL == error) {
		goto err_malloc;
	}
	if (NULL == message) {
		error->message = NULL;
	} else {
		error->message = strdup(message);
		if (NULL == error->message) {
			goto err_strdup_message;
		}
	}
	error->type = type;
	return error;
err_strdup_message:
	free(error);
err_malloc:
	return NULL;
}

void error_free(Error *error)
{
	if (NULL != error->message) {
		free(error->message);
	}
	free(error);
}

/** 列挙体を文字列に対応させるための構造体 */
typedef struct _ErrorString ErrorString;
struct _ErrorString {
	ErrorType type;
	char *str;
};

ErrorString error_strings[] = {
	{ EmptyStackError, "EmptyStackError" },
	{ IllegalTypeError, "IllegalTypeError" },
	{ DividedByZeroError, "DividedByZeroError" },
};

char *error_str(Error const *error, char *buffer, size_t size)
{
	if (NULL != error->message) {
		snprintf(buffer, size, "%s: %s",
				 error_strings[error->type].str, error->message);
	} else {
		snprintf(buffer, size, "%s",
				 error_strings[error->type].str);
	}
	return buffer;
}
