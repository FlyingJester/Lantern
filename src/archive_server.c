#include <stdlib.h> /* For NULL. */

/* Include for alloca */
#ifdef _MSC_VER
#include <malloc.h>
#endif
#ifdef __GNUC__
#include <alloca.h>
#endif

#include <assert.h>

/*----------------------------------------------------------------------------*/
#include "fileops/fileops.h"
#include "bufferfile/bufferfile.h"

#include "lantern_attributes.h"

/* Forward declares */
/*============================================================================*/
void *Lantern_CreateArchiveServer();

/*----------------------------------------------------------------------------*/
void Lantern_DestroyArchiveServer(void *server);

/*----------------------------------------------------------------------------*/
void Lantern_AppendToArchiveServer(void *server,
    const void *data, unsigned long size);

/* Static data */
/*============================================================================*/
static void *server = NULL;

/*----------------------------------------------------------------------------*/
#define NUMDIRS 3
static const char *const paths[NUMDIRS] = {
    "textures/",
    "images/",
    "rooms/"
};

static const unsigned path_sizes[NUMDIRS] = {
    9,
    7,
    6
};

/* Functions */
/*============================================================================*/
typedef const struct Lantern_FileFinder *const ff;
static void LANTERN_NOINLINE(lantern_low_append_archive(ff finder, unsigned p));
static void lantern_low_append_archive(ff finder, unsigned p){
    const char *const finder_path = Lantern_FileFinderPath(finder);
    unsigned i = 0;
    
    while(finder_path[i])
        i++;
    {
        const unsigned finder_path_size = i;
        char *const fullpath = alloca(finder_path_size + path_sizes[p] + 1);
        
        for(i = 0; i < path_sizes[p]; i++){
            fullpath[i] = paths[p][i];
        }
        for(i = 0; i < finder_path_size; i++){
            fullpath[i + path_sizes[p]] = finder_path[i];
        }
        
        fullpath[i] = '\0';
        
        {
            int size;
            void *const data = BufferFile(fullpath, &size);
            
            if(data == NULL || size <= 0)
                return;
            
            Lantern_AppendToArchiveServer(server, data, size);
            
            FreeBufferFile(data, size);
        }
    }
}

void *lantern_low_create_global_archive(){
    struct Lantern_FileFinder *const finder =
        (struct Lantern_FileFinder *)alloca(Lantern_FileFinderSize());
    unsigned i;

#ifndef NDEBUG
    for(i = 0; i < NUMDIRS; i++){
        unsigned e;
        for(e = 0; paths[i][e] != 0; e++){}
        assert(e == path_sizes[i] + 1);
    }
#endif
    
    assert(server == NULL);
    server = Lantern_CreateArchiveServer();
    
    for(i = 0; i < NUMDIRS; i++){
        if(!Lantern_InitFileFinder(finder, paths[i]))
            continue;
        do{
            lantern_low_append_archive(finder, i);
        }while(Lantern_FileFinderNext(finder));
        Lantern_DestroyFileFinder(finder);
    }
    
    return server;
}

/*----------------------------------------------------------------------------*/
void lantern_low_destroy_global_archive(){
    assert(server);
    Lantern_DestroyArchiveServer(server);
    server = NULL;
}
