#pragma once

#include "src/lantern_attributes.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Lantern_FileFinder;

unsigned LANTERN_CONST(Lantern_FileFinderSize());
bool LANTERN_FASTCALL Lantern_InitFileFinder(struct Lantern_FileFinder *finder, const char *path);
void LANTERN_FASTCALL Lantern_DestroyFileFinder(struct Lantern_FileFinder *finder);
const char *LANTERN_FASTCALL LANTERN_CONST(Lantern_FileFinderPath(const struct Lantern_FileFinder *finder));
unsigned long LANTERN_FASTCALL LANTERN_CONST(Lantern_FileFinderFileSize(const struct Lantern_FileFinder *finder));
bool LANTERN_FASTCALL Lantern_FileFinderNext(struct Lantern_FileFinder *finder);

#ifdef __cplusplus
}
#endif
