#include "primitives.h"
#include "lantern_gl_ext.h"
#include "lantern_attributes.h"

#include "servers/archives.hpp"

#include "spherefonts/font.h"

#include "glow/glow.h"
#include "chrono/chrono.h"

#include <cstdlib>
#include <cstdio>
#include <cassert>

// This will call Lantern_Run
extern "C" int Lantern_Start();

extern "C" LANTERN_FASTCALL int Lantern_End(struct Glow_Window *window);

int glow_main(int argc, char *argv[]){
	if(argc || argv){}
	puts("Lantern Game Engine starting");
    fflush(stdout);

	return Lantern_Start();
}

static void lantern_frame_delay(){
    static unsigned microtime_frame_start = Lightning_GetMicrosecondsTime();
    
    const unsigned microtime_new_frame_start = Lightning_GetMicrosecondsTime();
    assert(microtime_frame_start <= microtime_new_frame_start + 1);
    
    if(microtime_new_frame_start < microtime_frame_start + 16666){
        Lightning_MicrosecondsSleep((microtime_frame_start + 16666) - microtime_new_frame_start);
    }
    microtime_frame_start = microtime_new_frame_start + 1;
}

// At this point the window is properly initialized and shown.
extern "C" LANTERN_FASTCALL
int Lantern_Run(struct Glow_Window *window, const Lantern::ArchiveServer *archive_server){
	
    printf("Got window %p and archive server %p\n", window, archive_server);
    fflush(stdout);
	
    Glow_Event event;
    event.type = eGlowUnknown;
    
    
    do {
        lantern_frame_delay();
        
        // Draw Scene
        
        Glow_FlipScreen(window);
        // Get Events
        while(Glow_GetEvent(window, &event)){
            
            
            
        }
        // Handle Event
        
    } while(event.type != eGlowQuit);
    
	Lantern_End(window);
	return EXIT_SUCCESS;
}
