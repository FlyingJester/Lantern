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
    
    LANTERN_CREATEPRIMTIVE(background0);
    LANTERN_CREATEPRIMTIVE(background1);
    LANTERN_CREATEPRIMTIVE(background2);
    
    LANTERN_CREATEPRIMTIVE(outline_ul);
    LANTERN_CREATEPRIMTIVE(outline_u);
    LANTERN_CREATEPRIMTIVE(outline_ur);
    LANTERN_CREATEPRIMTIVE(outline_r);
    LANTERN_CREATEPRIMTIVE(outline_dr);
    LANTERN_CREATEPRIMTIVE(outline_d);
    LANTERN_CREATEPRIMTIVE(outline_dl);
    LANTERN_CREATEPRIMTIVE(outline_l);

    const float text_h = 64.0f;
    const float h_repeat = 0.75 *  text_h / static_cast<float>(text_background_image.h);
    background0.UVRectangle(0.0f,  0.0f, 0.25f, h_repeat, 16, text_h, text_background_image);
    background1.UVRectangle(0.25f, 0.0f, 0.75f, h_repeat, Lantern::ScreenWidth - 64, text_h, text_background_image);
    background2.UVRectangle(0.75f, 0.0f, 1.0f,  h_repeat, 16, text_h, text_background_image);
    
    outline_ul.UVRectangle(0.0f, 0.0f, 0.5f, 0.5f, 16, 16, outline_image);
    outline_ur.UVRectangle(0.5f, 0.0f, 1.0f, 0.5f, 16, 16, outline_image);
    outline_dr.UVRectangle(0.5f, 0.5f, 1.0f, 1.0f, 16, 16, outline_image);
    outline_dl.UVRectangle(0.0f, 0.5f, 0.5f, 1.0f, 16, 16, outline_image);
    outline_u.UVRectangle(0.48f, 0.0f, 0.49f, 0.5f, Lantern::ScreenWidth - 32, 16, outline_image);
    outline_r.UVRectangle(0.0f, 0.48f, 0.5f, 0.49f, 16, text_h, outline_image);
    outline_d.UVRectangle(0.51f, 0.5f, 0.52f, 1.0f, Lantern::ScreenWidth - 32, 16, outline_image);
    outline_l.UVRectangle(0.5f, 0.51f, 1.0f, 0.52f, 16, text_h, outline_image);
    
    Lantern_AddTextToFontContext(ctx, "This is some text.", 16, 16);
    
    Glow_Event event;
    event.type = eGlowUnknown;
    
    const unsigned text_area_y = Lantern::ScreenHeight - (text_h + 16);
    do {
        lantern_frame_delay();
        
        // Draw Scene
        Lantern_DrawFontContext(ctx);
        background0.draw(16, text_area_y);
        background1.draw(32, text_area_y);
        background2.draw(Lantern::ScreenWidth - 32, text_area_y);
        outline_ul.draw(0, text_area_y - 16);
        outline_ur.draw(Lantern::ScreenWidth - 16, text_area_y - 16);
        outline_dr.draw(Lantern::ScreenWidth - 16, Lantern::ScreenHeight - 16);
        outline_dl.draw(0, Lantern::ScreenHeight - 16);
        
        outline_u.draw(16, text_area_y - 16);
        outline_l.draw(0, text_area_y);
        outline_d.draw(16, Lantern::ScreenHeight - 16);
        outline_r.draw( Lantern::ScreenWidth - 16, text_area_y);
        
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
