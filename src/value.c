/*
 * Copyright 2012 Yuichi Araki. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "forsh.h"

Value *value_new_integer(int i)
{
	Value *value;
	value = (Value *) malloc(sizeof(Value));
	if (NULL == value) {
		return NULL;
	}
	value->type = TYPE_INTEGER;
	value->data = (void *) i;
	return value;
}

Value *value_new_integer_str(char const *str)
{
	return value_new_integer(atoi(str));
}

void value_free(Value *value)
{
	free(value);
}

int value_integer_value(Value *integer)
{
	return (int) integer->data;
}

void value_str(Value const *value, char *buf, size_t size)
{
	switch (value->type) {
	case TYPE_INTEGER:
		snprintf(buf, size, "%d", (int) value->data);
		break;
	case TYPE_FUNCTION:
		snprintf(buf, size, "FUNC(%d)", (int) value->data);
		break;
	}
}

Value *value_new_function(ForshFunc *func)
{
	Value *value;
	value = (Value *) malloc(sizeof(Value));
	if (NULL == value) {
		return NULL;
	}
	value->type = TYPE_FUNCTION;
	value->data = func;
	return value;
}

ForshFunc *value_function(Value const *value)
{
	return value->data;
}

