/*
 * Copyright 2012 Yuichi Araki. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "forsh.h"

static bool is_valid_symbol(char const *name);

void value_str(Value const *value, char *buf, size_t size)
{
	switch (value->type) {
	case TYPE_INTEGER:
		snprintf(buf, size, "%d", (int) value->data);
		break;
	case TYPE_FUNCTION:
		snprintf(buf, size, "FUNC(%d)", (int) value->data);
		break;
	case TYPE_SYMBOL:
		snprintf(buf, size, "%s", (char *) value->data);
		break;
	}
}

void value_free(Value *value)
{
	if (NULL == value) { return; }
	if (value->type == TYPE_SYMBOL) {
		free(value->data);
	}
	free(value);
}

// ==================================================
// 整数

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

int value_integer_value(Value *integer)
{
	return (int) integer->data;
}

// ==================================================
// 関数

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

// ==================================================
// シンボル

Value *value_new_symbol(char const *name)
{
	Value *value;
	if (!is_valid_symbol(name)) {
		return NULL;
	}
	value = (Value *) malloc(sizeof(Value));
	if (NULL == value) {
		return NULL;
	}
	value->type = TYPE_SYMBOL;
	value->data = strdup(name);
	if (NULL == value->data) {
		goto err_strdup;
	}
	return value;
err_strdup:
	free(value);
	return NULL;
}

char const *value_symbol_name(Value const *value)
{
	return (char const *) value->data;
}

static bool is_valid_symbol(char const *name)
{
	if (NULL == name) { return FALSE; }
	if (strlen(name) < 1) { return FALSE; }
	if (!isalpha(name[0])) { return FALSE; }
	++name;
	while (*name) {
		if (!isalnum(*name)) {
			return FALSE;
		}
		++name;
	}
	return TRUE;
}
