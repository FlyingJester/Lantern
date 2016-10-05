#include "fileops.h"

#include <Windows.h>

#include <assert.h>
#include <stdio.h>

struct Lantern_FileFinder{
	WIN32_FIND_DATA data;
	HANDLE find;
};

unsigned Lantern_FileFinderSize(){
	return sizeof(struct Lantern_FileFinder);
}

static bool file_attrs_are_ok(DWORD attrs){
    return (attrs == FILE_ATTRIBUTE_NORMAL ||
        (attrs & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_ARCHIVE)) == attrs);
}

bool Lantern_InitFileFinder(struct Lantern_FileFinder *finder, const char *path){
	
    fputs("Inited file finder for ", stdout);
    puts(path);
    fflush(stdout);
    
	char lpath[0x103];
	unsigned i = 0;
	
	if(path == NULL || path[0] == '\0'){
		lpath[0] = '*';
		lpath[1] = '\0';
	}
	else{
	
		while(path[i] != '\0' && i + 2 < sizeof(lpath)){
			lpath[i] = path[i];
			i++;
		}
		
		if(path[i-1] != '/')
			lpath[i] = '/';
		else
			i--;
		lpath[i+1] = '*';
		lpath[i+2] = '\0';
	}
	
	if ((finder->find = FindFirstFileA(lpath, &finder->data)) == INVALID_HANDLE_VALUE)
        return false;
    while((!file_attrs_are_ok(finder->data.dwFileAttributes)) || finder->data.cFileName[0] == '.'){
		if(FindNextFile(finder->find, &finder->data) == 0)
			return false;
    }
    return true;
}

const char *Lantern_FileFinderPath(const struct Lantern_FileFinder *finder){
    fputs("Getting file finder path ", stdout);
    puts(finder->data.cFileName);
    fflush(stdout);
	return finder->data.cFileName;
}

unsigned long Lantern_FileFinderFileSize(const struct Lantern_FileFinder *finder){
	return finder->data.nFileSizeLow;
}

bool Lantern_FileFinderNext(struct Lantern_FileFinder *finder){
    puts("Getting next file inf file finder");
    fflush(stdout);
	do{
        puts("Examinig file");
        fflush(stdout);
		if(FindNextFile(finder->find, &finder->data) == 0)
			return false;
	}while((!file_attrs_are_ok(finder->data.dwFileAttributes)) || finder->data.cFileName[0] == '.');
    fputs("Using file ", stdout);
    puts(finder->data.cFileName);
    fflush(stdout);
	return true;
}

void Lantern_DestroyFileFinder(struct Lantern_FileFinder *finder){
	FindClose(finder->find);
}
