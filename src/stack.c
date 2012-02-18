/*
 * Copyright 2012 Yuichi Araki. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "forsh.h"

/**
 * スタックのメモリの長さを拡大する。
 * \stack スタック
 */
static bool stack_realloc(Stack *stack);

Stack *stack_new(FreeFunc *value_free)
{
	Stack *stack;
	stack = (Stack *) malloc(sizeof(Stack));
	if (NULL == stack) {
		goto err_malloc;
	}
	stack->memlen = 16;
	stack->values = (void **) malloc(sizeof(void *) * stack->memlen);
	if (NULL == stack->values) {
		goto err_malloc_values;
	}
	stack->len = 0;
	stack->value_free = value_free;
	return stack;
err_malloc_values:
	free(stack);
err_malloc:
	return NULL;
}

void stack_free(Stack *stack)
{
	FreeFunc *value_free;
	int i;
	if (stack->value_free) {
		value_free = stack->value_free;
	} else {
		value_free = free;
	}
	for (i = 0; i < stack->len; ++i) {
		value_free(stack->values[i]);
	}
	free(stack->values);
	free(stack);
}

static bool stack_realloc(Stack *stack)
{
	stack->memlen *= 2;
	return NULL != realloc(stack->values, sizeof(void *) * stack->memlen);
}

void stack_each(Stack *stack, void (*func)(void*))
{
	int i;
	for (i = 0; i < stack->len; ++i) {
		func(stack->values[i]);
	}
}

bool stack_push(Stack *stack, void *value)
{
	if (stack->memlen <= stack->len) {
		if (!stack_realloc(stack)) {
			return FALSE;
		}
	}
	stack->values[stack->len] = value;
	stack->len += 1;
	return TRUE;
}

void *stack_pop(Stack *stack)
{
	if (stack->len == 0) { return NULL; }
	stack->len -= 1;
	return stack->values[stack->len];
}
