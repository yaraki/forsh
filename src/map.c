/*
 * Copyright 2012 Yuichi Araki. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "forsh.h"

/**
 * Pair の新しいインスタンスを生成する。
 * \key キー
 * \value 値
 */
static Pair *pair_new(const char *key, void *value);

/**
 * Pair を解放する
 * \pair ペア
 */
static void pair_free(Pair *pair);

/**
 * Map のメモリの長さを拡大する。
 * \map マップ
 */
static bool map_realloc(Map *map);

static Pair *pair_new(const char *key, void *value)
{
	Pair *pair;
	pair = (Pair *) malloc(sizeof(Pair));
	pair->key = strdup(key);
	pair->value = value;
	return pair;
}

static void pair_free(Pair *pair)
{
	free(pair->key);
	free(pair);
}

Map *map_new(void (*value_free)(void *))
{
	Map *map;
	map = (Map *) malloc(sizeof(Map));
	if (NULL == map) {
		goto err_malloc;
	}
	map->memlen = 16;
	map->pairs = (Pair **) malloc(sizeof(Pair *) * map->memlen);
	if (NULL == map->pairs) {
		goto err_malloc_pairs;
	}
	map->len = 0;
	map->value_free = value_free;
	return map;
err_malloc_pairs:
	free(map);
err_malloc:
	return NULL;
}

FreeFunc *map_value_free(Map const *map)
{
	FreeFunc *value_free;
	if (map->value_free) {
		value_free = map->value_free;
	} else {
		value_free = free;
	}
	return value_free;
}

void map_free(Map *map)
{
	int i;
	FreeFunc *value_free;
	value_free = map_value_free(map);
	for (i = 0; i < map->len; ++i) {
		value_free(map->pairs[i]->value);
		pair_free(map->pairs[i]);
	}
	free(map);
}

static bool map_realloc(Map *map)
{
	map->memlen *= 2;
	return NULL != realloc(map->pairs, sizeof(Pair *) * map->memlen);
}

bool map_put(Map *map, char const *key, void *value)
{
	int i;
	Pair *pair;
	FreeFunc *value_free;
	/* すでにキーが存在する場合は上書きする */
	for (i = 0; i < map->len; ++i) {
		pair = map->pairs[i];
		if (0 == strcmp(pair->key, key)) {
			value_free = map_value_free(map);
			value_free(pair->value);
			pair->value = value;
			return TRUE;
		}
	}
	/* キーが見つからなかった場合は追加する */
	if (map->memlen <= map->len) {
		if (!map_realloc(map)) {
			return FALSE;
		}
	}
	map->pairs[map->len] = pair_new(key, value);
	map->len += 1;
	return TRUE;
}

void *map_get(Map const *map, char const *key)
{
	int i;
	Pair *pair;
	for (i = 0; i < map->len; ++i) {
		pair = map->pairs[i];
		if (0 == strcmp(pair->key, key)) {
			return pair->value;
		}
	}
	return NULL;
}
