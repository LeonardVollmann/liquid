#define USAGE "USAGE: asset_compile [asset_filename] [out_filename]\n\n\
This program compiles assets (textures, shaders, models, etc) into a single binary blob\n\
that is then loaded at startup by programs using the liquid library.\n\
The supply the program with a text file listing the assets to be loaded using\n\
the following syntax:\n\
NAME_1 TYPE_1 RELATIVE_PATH_TO_ASSET_1\n\
NAME_2 TYPE_2 RELATIVE_PATH_TO_ASSET_2\n\
NAME_3 TYPE_3 RELATIVE_PATH_TO_ASSET_3\n\n\
TYPE is either TEXT, SHADER, IMAGE or MODEL\
"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "common.h"

typedef enum
{
	TEXT,
	IMAGE,
	SHADER,
	MODEL
} AssetType;

typedef struct
{
	u64 magic_number;
	AssetType type;
	size_t size;
} AssetHeader;

typedef struct
{
	char filename[256];
	char name[256];
	AssetType type;
} AssetFile;

static char *read_file(const char *filename)
{
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		FATAL("Could not open file: %s", filename);
	}
	INFO("Loaded file: %s", filename);

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);
	char *result = malloc(size + 1);
	fread(result, size, 1, file);
	fclose(file);
	*(result + size) = 0;

	return result;
}

static char *parse_line(char *line_start, AssetFile *file)
{
	char *name_start = line_start;
	char *ptr = name_start;
	while (isgraph(*ptr)) { ptr++; }
	*ptr = 0;
	if (strlen(name_start) > 255) {
		ERROR("Asset name is too long. Must be less than 255 characters. Name: %s", name_start);
		return NULL;
	}
	strcpy(file->name, name_start);

	while (!isgraph(*ptr)) { ptr++; }
	char *type_start = ptr;
	ptr = type_start;
	while (isgraph(*ptr)) { ptr++; }
	*ptr = 0;
	if (strcmp(type_start, "TEXT") == 0) {
		file->type = TEXT;
	} else if (strcmp(type_start, "SHADER") == 0) {
		file->type = SHADER;
	} else if (strcmp(type_start, "IMAGE") == 0) {
		file->type = IMAGE;
	} else if (strcmp(type_start, "MODEL") == 0) {
		file->type = MODEL;
	} else {
		ERROR("Asset type \'%s\' is invalid. Must be one of: TEXT, SHADER, MODEL, IMAGE.", type_start);
		return NULL;
	}

	while (!isgraph(*ptr)) { ptr++; }
	char *filename_start = ptr;
	ptr = filename_start;
	while (isgraph(*ptr)) { ptr++; }
	*ptr = 0;
	if (strlen(filename_start) > 255) {
		ERROR("Asset filename is too long. Must be less than 255 characters. Name: %s", name_start);
		return NULL;
	}
	strcpy(file->filename, filename_start);

	while (*ptr && *ptr != '\n') {
		ptr++;
	}
	// if (*ptr == '\n') ptr++;

	return ptr;
}

int main(int argc, char const *argv[])
{
	if (argc != 3) {
		printf("%s\n", USAGE);
		return 1;
	}

	const char *asset_filename = argv[1];
	const char *out_filename = argv[2];

	char *asset_config = read_file(asset_filename);

	u32 file_cap = 2;
	AssetFile *files = malloc(sizeof(AssetFile) * file_cap);
	u32 num_files = 0;

	u32 line_number = 0;
	char *line_start = asset_config;
	while (*line_start) {
		line_number++;
		if (*line_start == '\n' || *line_start == '#') { // Skip empty and escaped lines
			line_start++;
			continue;
		}

		line_start = parse_line(line_start, files + num_files);
		if (line_start == NULL) {
			ERROR("Error parsing line %u. Exiting program.", line_number);
			free(files);
			free(asset_config);
			return 1;
		}

		printf("Parsed asset: %s, %u, %s\n", files[num_files].name, files[num_files].type, files[num_files].filename);

		num_files++;
		if (num_files == file_cap) {
			file_cap *= 2;
			files = realloc(files, sizeof(AssetFile) * file_cap);
		}
	}

	return 0;
}