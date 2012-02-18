/*
 * Copyright 2012 Yuichi Araki. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TRUE
#define TRUE !0
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef bool
#define bool int
#endif

/** free 関数のシグネチャ */
typedef void FreeFunc(void *);

/** スタック */
typedef struct _Stack Stack;
struct _Stack {
	void **values;         /* 要素 */
	size_t len;            /* 長さ */
	size_t memlen;         /* 確保されているメモリの長さ */
	FreeFunc *value_free;  /* 要素を解放する際に用いる関数 */
};

/** 文字列をキーとするペア */
typedef struct _Pair Pair;
struct _Pair {
	char *key;    /* キー */
	void *value;  /* 値 */
};

/** 文字列をキーとするマップ */
typedef struct _Map Map;
struct _Map {
	Pair **pairs;          /* 要素 */
	size_t len;            /* 長さ */
	size_t memlen;         /* 確保されているメモリの長さ */
	FreeFunc *value_free;  /* 要素を解放する際に用いる関数 */
};

/** 型 */
typedef enum _Type Type;
enum _Type {
	TYPE_INTEGER,   /* 整数 */
	TYPE_FUNCTION,  /* 関数 */
};

/** 値 */
typedef struct _Value Value;
struct _Value {
	Type type;   /* このインスタンスの型 */
	void *data;  /* データ */
};

/** 文脈 */
typedef struct _Context Context;
struct _Context {
	Stack *stack;  /* スタック */
	Map *map;      /* シンボル・テーブル */
};

/** エラー種別 */
typedef enum _ErrorType ErrorType;
enum _ErrorType {

	EmptyStackError,     /* スタックが空である */
	IllegalTypeError,    /* 型が不正である */
	DividedByZeroError,  /* ゼロによる割り算 */
};

/** エラー */
typedef struct _Error Error;
struct _Error {
	ErrorType type;  /* エラー種別 */
	char *message;   /* メッセージ */
};

/* Forsh の関数 */
typedef Error *ForshFunc(Stack *stack);

/* stack.c */
/**
 * Stack の新しいインスタンスを生成する。
 * \free_func 要素を解放する際に用いるべき関数
 */
Stack *stack_new(void (*free_func)(void *));

/**
 * Stack を解放する。
 * \stack スタック
 */
void stack_free(Stack *stack);

/**
 * スタック内のそれぞれの要素に対して関数を実行する。
 * \stack スタック
 * \func 実行する関数
 */
void stack_each(Stack *stack, void (*func)(void*));

/**
 * スタックに要素を追加する。
 * \stack スタック
 * \value 追加する要素
 */
bool stack_push(Stack *stack, void *value);

/**
 * スタックから要素を取り出す。返された値は呼び出し側で適切に解放されね
 * ばならない。
 * \stack スタック
 */
void *stack_pop(Stack *stack);

/* map.c */
/**
 * Map の新しいインスタンスを生成する。
 * \value_free 値を解放する際に用いるべき関数。
 */
Map *map_new(void (*value_free)(void *));

/**
 * マップに格納されている値を解放する際に用いるべき関数を返す。
 * \map マップ
 */
FreeFunc *map_value_free(Map const *map);

/**
 * マップを解放する。
 * \map マップ
 */
void map_free(Map *map);

/**
 * マップに要素を加える。指定したキーに結びついた値がある場合は上書きさ
 * れる。成功した場合は TRUE、失敗した場合は FALSE を返す。
 * \key キー
 * \value 値
 */
bool map_put(Map *map, char const *key, void *value);

/**
 * マップからキーに対応する値を返す。返された値を呼び出し側で解放しては
 * ならない。
 * \map マップ
 * \key キー
 */
void *map_get(Map const *map, char const *key);

/* value.c */
/**
 * Value の新しいインスタンスを整数として生成する。
 * \i 整数
 */
Value *value_new_integer(int i);

/**
 * Value の新しいインスタンスを整数として生成する。
 * \str 整数の文字列表現
 */
Value *value_new_integer_str(char const *str);

/**
 * Value を解放する。
 * \value 解放する値
 */
void value_free(Value *value);

/**
 * Value の整数としての値を取得する。
 * \integer 整数として作られた Value のインスタンス
 */
int value_integer_value(Value *integer);

/**
 * Value の文字列表現を取得する。
 * \buf 文字列の書き込み先
 * \size 書き込み文字数の制限値
 */
void value_str(Value const *value, char *buf, size_t size);

/**
 * Value の新しいインスタンスを関数として生成する。
 * \func 関数
 */
Value *value_new_function(ForshFunc *func);

/**
 * Value の関数としての値を取得する
 * \value 関数として作られた Value のインスタンス
 */
ForshFunc *value_function(Value const *value);

/* builtin.c */
/** '+' を実装する */
Error *forsh_plus(Stack *stack);
/** '-' を実装する */
Error *forsh_minus(Stack *stack);
/** '*' を実装する */
Error *forsh_star(Stack *stack);
/** '/' を実装する */
Error *forsh_slash(Stack *stack);

/* context.c */
/**
 * Context の新しいインスタンスを生成する。
 */
Context *context_new(void);

/**
 * Context を解放する。
 * \context 解放する Context
 */
void context_free(Context *context);

/**
 * Context の内容を表示する。
 * \context 表示する Context
 */
void context_describe(Context const *context);

/**
 * Context にトークンを解釈させる。
 * \context 文脈
 * \str 解釈するトークン文字列
 */
void context_interpret(Context *context, const char *str);

/* error.c */
/**
 * Error の新しいインスタンスを初期化する
 * \type エラー種別
 * \message エラーメッセージ。指定しない場合は NULL とする。
 */
Error *error_new(ErrorType type, char const *message);

/**
 * Error を解放する。
 * \error エラー
 */
void error_free(Error *error);

/**
 * Error の文字列表現を取得する。
 * \error エラー
 * \buffer 文字列の書き込み先
 * \size 書き込み文字数の制限値
 */
char *error_str(Error const *error, char *buffer, size_t size);

