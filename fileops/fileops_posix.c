#include "fileops.h"

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

struct Lantern_FileFinder{
    struct dirent *dirt;
    DIR *dir;
};

unsigned Lantern_FileFinderSize(){
    return sizeof(struct Lantern_FileFinder);
}

bool Lantern_InitFileFinder(struct Lantern_FileFinder *finder, const char *path){
    return finder && path && (finder->dir = opendir(path)) != NULL;
}

void Lantern_DestroyFileFinder(struct Lantern_FileFinder *finder){
    closedir(finder->dir);
}

const char *Lantern_FileFinderPath(const struct Lantern_FileFinder *finder){
    return finder->dirt ? finder->dirt->d_name : "";
}

unsigned long Lantern_FileFinderFileSize(const struct Lantern_FileFinder *finder){
    struct stat st;
    
    if(!finder->dirt)
        return 0;
    
    stat(finder->dirt->d_name, &st);
    return st.st_size;
}

bool Lantern_FileFinderNext(struct Lantern_FileFinder *finder){
    return (finder->dirt = readdir(finder->dir)) != NULL;
}
