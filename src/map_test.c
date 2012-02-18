/*
 * Copyright 2012 Yuichi Araki. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "forsh.h"

static char *lastly_freed;

static void free_remember(void *p)
{
	lastly_freed = strdup((char *) p);
	free(p);
}

int main(int argc, char **argv)
{
	Map *map;
	char *value;
	bool ok = TRUE;
	map = map_new(free_remember);
	map_put(map, "dog", strdup("inu"));
	map_put(map, "cat", strdup("neko"));
	map_put(map, "duck", strdup("ahiru"));
	value = (char *) map_get(map, "dog");
	if (NULL == value) {
		printf("expected: [[inu]], received: NULL\n");
		ok = FALSE;
	} if (0 != strcmp("inu", value)) {
		printf("expected: [[inu]], received: [[%s]]\n", value);
		ok = FALSE;
	}
	map_put(map, "cat", strdup("NEKO"));
	if (0 != strcmp("neko", lastly_freed)) {
		printf("expected: [[neko]], received: [[%s]]\n", lastly_freed);
	}
	if (3 != map->len) {
		printf("expected: 3, received: %lu\n", map->len);
		ok = FALSE;
	}
	value = (char *) map_get(map, "cat");
	if (NULL == value) {
		printf("expected: [[NEKO]], received: NULL\n");
		ok = FALSE;
	} if (0 != strcmp("NEKO", value)) {
		printf("expected: [[NEKO]], received: [[%s]]\n", value);
		ok = FALSE;
	}
	if (ok) {
		puts("OK");
	}
	free(lastly_freed);
	map_free(map);
}

