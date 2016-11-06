#include <stdio.h>
#include <stdlib.h>

#include "file.h"

char *loadFile(const char *name)
{
	FILE *file = fopen(name, "r");
	if (!file) {
		return NULL;
	}

	if (fseek(file, 0L, SEEK_END) == -1) {
		fclose(file);
		return NULL;
	}

	int len;
	if ((len = ftell(file)) == -1) {
		fclose(file);
		return NULL;
	}
	rewind(file);

	char *buf = malloc((len + 1) * sizeof *buf);
	if (!buf) {
		fclose(file);
		return NULL;
	}

	if (fread(buf, sizeof *buf, len, file) != len) {
		free(buf);
		fclose(file);
		return NULL;
	}

	buf[len] = '\0';
	fclose(file);
	return buf;
}

