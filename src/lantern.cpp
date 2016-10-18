#include "primitives.h"
#include "primitives_obj.hpp"
#include "lantern_gl_ext.h"
#include "lantern_attributes.h"

#include "draw_text.h"

#include "servers/archives.hpp"
#include "servers/textures.hpp"

#include "spherefonts/font.h"

#include "glow/glow.h"
#include "chrono/chrono.h"

#include <cstdlib>
#include <cstdio>
#include <cassert>

// This will call Lantern_Run
extern "C" int Lantern_Start();

extern "C" LANTERN_FASTCALL int Lantern_End(struct Glow_Window *window);

namespace Lantern {
extern const unsigned ScreenWidth, ScreenHeight;
}

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
    LX_Ortho(Lantern::ScreenWidth, Lantern::ScreenHeight);
    LX_EnableBlending();
    LX_EnableTexture();

    Lantern::TextureServer texture_server(*archive_server);

    Lantern::Image text_background_image = texture_server.load("windowstyle1.tga"),
        outline_image = texture_server.load("windowstyle.png");

    Lantern_FontContext *const ctx = Lantern_CreateFontContext();
    
    Lantern::WindowStyle ws(texture_server, Lantern::ScreenWidth, 96),
        items(texture_server, 96, Lantern::ScreenHeight - ws.h());
    
    Lantern_AddTextToFontContext(ctx, "This is some text.", 16, 16);
    
    Glow_Event event;
    event.type = eGlowUnknown;
    
    do {
        lantern_frame_delay();
        
        // Draw Scene
        Lantern_DrawFontContext(ctx);
        
        ws.draw(0, Lantern::ScreenHeight - ws.h());
        items.draw(Lantern::ScreenWidth - items.w(), 0);
        
        Glow_FlipScreen(window);
        // Get Events
        while(Glow_GetEvent(window, &event)){
            
            
            
        }
        // Handle Event
        
    } while(event.type != eGlowQuit);
    
    Lantern_DestroyFontContext(ctx);
    
	Lantern_End(window);
	return EXIT_SUCCESS;
}
