/*
 * Copyright 2012 Yuichi Araki. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "forsh.h"

/** 足し算 */
static int add(int a, int b);
/** 引き算 */
static int subtract(int a, int b);
/** 掛け算 */
static int multiply(int a, int b);
/** 割り算 */
static int divide(int a, int b);

/**
 * 二つの整数を引数に取る関数を Forsh の関数として実行する。
 * \stack スタック
 * \func 実行する関数
 * \nozero 二つ目の引数 (= スタックから1つ目に下ろした値) がゼロである
 * ときにエラーとするなら TRUE。割り算用。
 */
static Error *two_integer_func(Stack *stack,
							   int (*func)(int, int),
							   bool nozero);

static int add(int a, int b)
{
	return a + b;
}

static int subtract(int a, int b)
{
	return a - b;
}

static int multiply(int a, int b)
{
	return a * b;
}

static int divide(int a, int b)
{
	return a / b;
}

static Error *two_integer_func(Stack *stack,
							   int (*func)(int, int),
							   bool nozero)
{
	Value *a, *b;
	int i;
	Error *error;
	a = stack_pop(stack);
	if (NULL == a) {
		error = error_new(EmptyStackError, NULL);
		goto err_empty_a;
	}
	if (a->type != TYPE_INTEGER) {
		error = error_new(IllegalTypeError, NULL);
		goto err_invalid_a;
	}
	if (nozero && 0 == value_integer_value(a)) {
		error = error_new(DividedByZeroError, NULL);
		goto err_invalid_a;
	}
	b = stack_pop(stack);
	if (NULL == b) {
		error = error_new(EmptyStackError, NULL);
		goto err_empty_b;
	}
	if (b->type != TYPE_INTEGER) {
		error = error_new(IllegalTypeError, NULL);
		goto err_invalid_b;
	}
	/* スタックから下ろしたのと逆にして計算する必要がある */
	i = func(value_integer_value(b), value_integer_value(a));
	value_free(a);
	value_free(b);
	stack_push(stack, value_new_integer(i));
	return NULL;
err_invalid_b:
	stack_push(stack, b);
err_empty_b:
err_invalid_a:
	stack_push(stack, a);
err_empty_a:
	return error;
}

Error *forsh_plus(Stack *stack)
{
	return two_integer_func(stack, add, FALSE);
}

Error *forsh_minus(Stack *stack)
{
	return two_integer_func(stack, subtract, FALSE);
}

Error *forsh_star(Stack *stack)
{
	return two_integer_func(stack, multiply, FALSE);
}

Error *forsh_slash(Stack *stack)
{
	return two_integer_func(stack, divide, TRUE);
}

