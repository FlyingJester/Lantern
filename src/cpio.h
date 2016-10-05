#pragma once

#include "lantern_attributes.h"

#include <stdbool.h>

struct Lantern_Archive {
	const void *data;
	unsigned long size;
};

struct Lantern_ArchiveEntry {
	char name[0x100];
	unsigned start, length;
};

#define LANTERN_ARCHIVE_FILE_HEADER_LENGTH 76
#define LANTERN_ARCHIVE_INVALID_SIZE 0xDEADCAFE

#ifdef __cplusplus
extern "C" {
#endif

bool LANTERN_CONST(Lantern_ArchiveFileMatchesName(const struct Lantern_Archive *data, unsigned at, const char *name));
/* Note that names should be null-terminated, and sizes include that. */
unsigned LANTERN_CONST(Lantern_ArchiveFileNameLength(const struct Lantern_Archive *data, unsigned at));
unsigned LANTERN_CONST(Lantern_ArchiveFileLength(const struct Lantern_Archive *data, unsigned at));

/* Entries that aren't found will have LANTERN_ARCHIVE_INVALID_SIZE as the start and length. */
void Lantern_ArchiveSearch(const struct Lantern_Archive *LANTERN_RESTRICT data,
	struct Lantern_ArchiveEntry *LANTERN_RESTRICT in_out, unsigned num_entries);

typedef bool (*lantern_enum_callback)(void *LANTERN_RESTRICT arg,
	const struct Lantern_Archive *LANTERN_RESTRICT archive,
	const struct Lantern_ArchiveEntry *LANTERN_RESTRICT file);
bool Lantern_ArchiveEnumerate(const struct Lantern_Archive *LANTERN_RESTRICT data,
	lantern_enum_callback, void *LANTERN_RESTRICT arg); 

#ifdef __cplusplus
}
#endif
