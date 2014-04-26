/*
 * Copyright 2012 Yuichi Araki. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "forsh.h"

/**
 * 文字列が整数として判別できる場合は TRUE を返す。
 * \str 文字列
 */
static bool str_is_integer(char const *str);

/**
 * 頭に空白を加えて値を表示する。
 * \value 表示する値
 */
static void print_value(Value *value);

/**
 * 文脈のシンボル・テーブルにビルトイン関数を束縛する。
 * \context 文脈
 */
static void context_builtin(Context *context);

static void context_builtin(Context *context)
{
	map_put(context->map, "+", value_new_function(forsh_plus));
	map_put(context->map, "-", value_new_function(forsh_minus));
	map_put(context->map, "*", value_new_function(forsh_star));
	map_put(context->map, "/", value_new_function(forsh_slash));
}

static void value_free_ignoring_symbols(Value *value)
{
	if (value->type != TYPE_SYMBOL) {
		value_free(value);
	}
}

Context *context_new(void)
{
	Context *context;
	context = (Context *) malloc(sizeof(Context));
	if (NULL == context) {
		goto err_malloc;
	}
	context->stack = stack_new((FreeFunc *) value_free_ignoring_symbols);
	if (NULL == context->stack) {
		goto err_malloc_stack;
	}
	context->map = map_new((FreeFunc *) value_free);
	if (NULL == context->map) {
		goto err_malloc_map;
	}
	context_builtin(context);
	context->defining_variable = FALSE;
	return context;
err_malloc_map:
	free(context->stack);
err_malloc_stack:
	free(context);
err_malloc:
	return NULL;
}

void context_free(Context *context)
{
	stack_free(context->stack);
	map_free(context->map);
	free(context);
}

void context_describe(Context const *context)
{
	putchar('#');
	stack_each(context->stack, (void (*)(void *)) print_value);
	putchar('\n');
}

Value *context_resolve(Context const *context, char const *key)
{
	return map_get(context->map, key);
}

Error *context_interpret(Context *context, const char *str)
{
	Value *value;
	if (context->defining_variable) {  // 変数定義
		context->defining_variable = FALSE;
		value = value_new_symbol(str);
		if (NULL == value) {  // エラー (変数名不正など)
			return error_new(IllegalVariableError, NULL);
		}
		map_put(context->map, value_symbol_name(value), value);
	} else if (str_is_integer(str)) {  // 整数
		value = value_new_integer_str(str);
		stack_push(context->stack, value);
	} else if (0 == strcmp(str, "VARIABLE")) {  // 変数定義の開始
		context->defining_variable = TRUE;
	} else if (NULL != (value = context_resolve(context, str))) {  // シンボル
		ForshFunc *func;
		switch (value->type) {
		case TYPE_FUNCTION:
			func = value_function(value);
			return func(context->stack);
		default:
			stack_push(context->stack, value);
			break;
		}
	} else {
		fprintf(stderr, "Failed to interpret: %s\n", str);
	}
	return NULL;
}

static bool str_is_integer(char const *str)
{
	if ('\0' == *str) { return FALSE; }
	while (*str) {
		if (!isdigit(*str)) {
			return FALSE;
		}
		++str;
	}
	return TRUE;
}

static void print_value(Value *value)
{
	char buf[1024];
	value_str(value, buf, sizeof(buf));
	printf(" %s", buf);
}
