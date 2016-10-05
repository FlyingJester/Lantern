#include "primitives.h"
#include "lantern_gl_ext.h"
#include "lantern_attributes.h"

#include "servers/archives.hpp"

#include "glow/glow.h"

#include <cstdlib>
#include <cstdio>

// This will call Lantern_Run
extern "C" int Lantern_Start();

extern "C" LANTERN_FASTCALL int Lantern_End(struct Glow_Window *window);

int glow_main(int argc, char *argv[]){
	if(argc || argv){}
	puts("Lantern Game Engine starting");
    fflush(stdout);

	return Lantern_Start();
}

// At this point the window is properly initialized and shown.
extern "C" LANTERN_FASTCALL int Lantern_Run(struct Glow_Window *window, const Lantern::ArchiveServer *archive_server){
	
    printf("Got window %p and archive server %p\n", window, archive_server);
    fflush(stdout);
	
	Lantern_End(window);
	return EXIT_SUCCESS;
}
