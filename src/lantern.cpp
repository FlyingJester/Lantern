#include "primitives.h"
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
        outline_image = texture_server.load("windowstyle.tga");

    Lantern_FontContext *const ctx = Lantern_CreateFontContext();
    Lantern_Primitive *const background = (Lantern_Primitive*)alloca(Lantern_PrimitiveSize()),
        *const outline0 = (Lantern_Primitive*)alloca(Lantern_PrimitiveSize()),
        *const outline1 = (Lantern_Primitive*)alloca(Lantern_PrimitiveSize()),
        *const outline2 = (Lantern_Primitive*)alloca(Lantern_PrimitiveSize());
    Lantern_InitPrimitive(background);
    Lantern_InitPrimitive(outline0);
    Lantern_InitPrimitive(outline1);
    Lantern_InitPrimitive(outline2);
    Lantern_CreateRectangle(background,
        text_background_image.w, text_background_image.h, text_background_image.texture);
    
    const float h_repeat = 96.0f / static_cast<float>(text_background_image.h);

    Lantern_CreateUVRectangle(0.0f, 0.0f, 0.25f, h_repeat,
        outline0, 16, 96, text_background_image.texture);

    Lantern_CreateUVRectangle(0.25f, 0.0f, 0.75f, h_repeat,
        outline1, Lantern::ScreenWidth - 64, 96, text_background_image.texture);

    Lantern_CreateUVRectangle(0.75f, 0.0f, 1.0f, h_repeat,
        outline2, 16, 96, text_background_image.texture);

    Lantern_AddTextToFontContext(ctx, "This is some text.", 16, 16);
    
    Glow_Event event;
    event.type = eGlowUnknown;
    
    do {
        lantern_frame_delay();
        
        // Draw Scene
        Lantern_DrawFontContext(ctx);
        Lantern_DrawPrimitive(background, 64, 64);
        Lantern_DrawPrimitive(outline0, 16, Lantern::ScreenHeight - 112);
        Lantern_DrawPrimitive(outline1, 32, Lantern::ScreenHeight - 112);
        Lantern_DrawPrimitive(outline2, Lantern::ScreenWidth - 32,
            Lantern::ScreenHeight - 112);
        
        Glow_FlipScreen(window);
        // Get Events
        while(Glow_GetEvent(window, &event)){
            
            
        }
        // Handle Event
        
    } while(event.type != eGlowQuit);
    
    Lantern_DestroyPrimitive(background);
    Lantern_DestroyPrimitive(outline0);
    Lantern_DestroyPrimitive(outline1);
    Lantern_DestroyPrimitive(outline2);
    
    Lantern_DestroyFontContext(ctx);
    
	Lantern_End(window);
	return EXIT_SUCCESS;
}
