/*
 
 To build:

  emcc -o sdl2_test.html sdl2_test.cpp -Wall -O3 -s USE_SDL=2 --preload-file img@ --emrun

 
 To run:
 
  emrun  --serve_after_close  sdl2_test.html
 
 
 
 Written by Rob Probin Sunday 16 Feb 2020.
 Some code modified from SDL2 demos written by Holmes Futrell "use however you want"
Fox Icon, PD from https://www.deviantart.com/omegazero22xx/art/8-bit-Fox-Sheet-541702756
 */

const START_VALUE = 100; //1500;

#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

#ifdef __EMSCRIPTEN__
// Put this at the top to import emscripten_set_main_loop()
#include "emscripten.h"
#endif

const int width = 800;
const int height = 600;
const int MAX_NUM_SPRITES = 1500;     /* number of faces to draw */
const int SPRITE_SIZE = 32;      /* width and height of fox sprite */

#define USE_BROWSER_VYSNC 1     // if zero, callback tries to be expected_framerate

const int expected_framerate = 60;
const int frames_per_print_framerate = 2*expected_framerate;

int num_sprites = START_VALUE;          // start value, should be <= MAX_NUM_SPRITES
int num_rects = 0;
int fixed_width_rect = true;   // leave this true and it automatically switches it off

// select what to do
#define SPRITE_BACKGROUND 0
#define ENABLE_COLOUR_KEY 0
#define ENABLE_BLEND_MODE 0
#define ENABLE_FOX 1

SDL_Renderer *renderer = NULL;
SDL_Texture *bitmapTex = NULL;
double deltaTime;

static struct
{
    float x, y;                 /* position of happyface */
    float xvel, yvel;           /* velocity of happyface */
    int r, g, b;
    int rw, rh;
} faces[MAX_NUM_SPRITES];


// --------------------------------------------------------------------------
int frame_count = 0;
int bump_count = 0;
double fr_sum = 0;
double fr_max = 0;
double fr_min = 9999999999;

void calc_framerate(double dt)
{
    double f = 1/dt;
    fr_sum += f;
    if(f > fr_max)
    {
        fr_max = f;
    }
    if(f < fr_min)
    {
        fr_min = f;
    }

    if(frame_count >= frames_per_print_framerate)
    {
        double fr_average = fr_sum / frames_per_print_framerate;
        printf("FPS = %f %f %f Sprites=%i Rects=%i\n", fr_min, fr_average, fr_max, num_sprites, num_rects);
        fr_sum = 0;
        fr_max = 0;
        fr_min = 9999999999;
        frame_count = 0;
        
        
        // alter the input parameters
        bump_count ++;
        if(bump_count >= 2)
        {
            bump_count = 0;
            if(num_rects == 0)
            {
                num_sprites += 100;
                if(num_sprites > MAX_NUM_SPRITES)
                {
                    num_sprites = 0;
                    num_rects = START_VALUE;
                    fixed_width_rect = true;
                    printf("Switch to fixed width rects\n");
                }
            }
            else
            {
                num_rects += 100;
                if(num_rects > MAX_NUM_SPRITES)
                {
                    if(fixed_width_rect)
                    {
                        num_sprites = 0;
                        num_rects = START_VALUE;
                        fixed_width_rect = false;
                        printf("Switch to variable rects\n");
                    }
                    else
                    {
                        num_sprites = START_VALUE;
                        num_rects = 0;
                        fixed_width_rect = true;
                        printf("Switch to sprites\n");
                    }
                }
            }
        }
    }
    frame_count ++;
}


// --------------------------------------------------------------------------
static Uint64 prevTime = 0;

double updateDeltaTime(void)
{
    Uint64 curTime = SDL_GetPerformanceCounter();

    double deltaTime;

    if (prevTime == 0) {
        prevTime = curTime;
    }
    deltaTime = (double) (curTime - prevTime) / (double) SDL_GetPerformanceFrequency();
    prevTime = curTime;

    calc_framerate(deltaTime);
    return deltaTime;
}

/*
    Produces a random int x, min <= x <= max
    following a uniform distribution
*/
int
randomInt(int min, int max)
{
    return min + rand() % (max - min + 1);
}

/*
    Produces a random float x, min <= x <= max
    following a uniform distribution
 */
float
randomFloat(float min, float max)
{
    return rand() / (float) RAND_MAX *(max - min) + min;
}

// --------------------------------------------------------------------------


void rect_test()
{
    Uint8 r, g, b;


    // not right for rects, but ok for now
    int maxx = width - SPRITE_SIZE;
    int maxy = height - SPRITE_SIZE;
    int minx = 0;
    int miny = 0;

    for (int i = 0; i < num_rects; i++) {
        faces[i].x += faces[i].xvel * deltaTime;
        faces[i].y += faces[i].yvel * deltaTime;
        if (faces[i].x > maxx) {
            faces[i].x = maxx;
            faces[i].xvel = -faces[i].xvel;
        } else if (faces[i].y > maxy) {
            faces[i].y = maxy;
            faces[i].yvel = -faces[i].yvel;
        }
        if (faces[i].x < minx) {
            faces[i].x = minx;
            faces[i].xvel = -faces[i].xvel;
        } else if (faces[i].y < miny) {
            faces[i].y = miny;
            faces[i].yvel = -faces[i].yvel;
        }
        
        /*  Come up with a random rectangle */
        SDL_Rect rect;
        if(fixed_width_rect)
        {
            rect.w = SPRITE_SIZE;
            rect.h = SPRITE_SIZE;
        }
        else
        {
            rect.w = faces[i].rw;
            rect.h = faces[i].rh;
        }
        rect.x = faces[i].x;
        rect.y = faces[i].y;

        /* Come up with a random color */
        r = faces[i].r;
        g = faces[i].g;
        b = faces[i].b;
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);


        /*  Fill the rectangle in the color */
        SDL_RenderFillRect(renderer, &rect);
    }
}

void sprite_test()
{
    int i;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    int w = width;
    int h = height;

    /* setup boundaries for happyface bouncing */
    int maxx = w - SPRITE_SIZE;
    int maxy = h - SPRITE_SIZE;
    int minx = 0;
    int miny = 0;

    /* setup rects for drawing */
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = SPRITE_SIZE;
    srcRect.h = SPRITE_SIZE;
    dstRect.w = SPRITE_SIZE;
    dstRect.h = SPRITE_SIZE;
    
    /*
       loop through all the happy faces:
       - update position
       - update velocity (if boundary is hit)
       - draw
     */
    for (i = 0; i < num_sprites; i++) {
        faces[i].x += faces[i].xvel * deltaTime;
        faces[i].y += faces[i].yvel * deltaTime;
        if (faces[i].x > maxx) {
            faces[i].x = maxx;
            faces[i].xvel = -faces[i].xvel;
        } else if (faces[i].y > maxy) {
            faces[i].y = maxy;
            faces[i].yvel = -faces[i].yvel;
        }
        if (faces[i].x < minx) {
            faces[i].x = minx;
            faces[i].xvel = -faces[i].xvel;
        } else if (faces[i].y < miny) {
            faces[i].y = miny;
            faces[i].yvel = -faces[i].yvel;
        }

        dstRect.x = faces[i].x;
        dstRect.y = faces[i].y;
        SDL_RenderCopy(renderer, bitmapTex, &srcRect, &dstRect);
    }

}
// --------------------------------------------------------------------------


/*
    Sets initial positions and velocities of happyfaces
    units of velocity are pixels per millesecond
*/
void initialize_sprites(SDL_Renderer *renderer)
{
    int i;
    int w = width;
    int h = height;
    //SDL_RenderGetLogicalSize(renderer, &w, &h);

    for (i = 0; i < MAX_NUM_SPRITES; i++) {
        faces[i].x = randomFloat(0.0f, w - SPRITE_SIZE);
        faces[i].y = randomFloat(0.0f, h - SPRITE_SIZE);
        faces[i].xvel = randomFloat(-60.0f, 60.0f);
        faces[i].yvel = randomFloat(-60.0f, 60.0f);
        
        // for rectangles
        faces[i].r = randomInt(50, 255);
        faces[i].g = randomInt(50, 255);
        faces[i].b = randomInt(50, 255);

        faces[i].rw = randomInt(64, 128);
        faces[i].rh = randomInt(64, 128);

    }
}


// The "main loop" function.
void one_iter() {
  // process input
  // render to screen

    deltaTime = updateDeltaTime();

    SDL_Event e;
    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            //break;
            exit(0);
        }
    }

    /* fill background in with black */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
#if SPRITE_BACKGROUND
    SDL_RenderCopy(renderer, bitmapTex, NULL, NULL);
#endif
    rect_test();
    sprite_test();
    SDL_RenderPresent(renderer);

}

// ---------------------------------------------------------------------------
std::string render_flag_decode(Uint32 rend)
{
    std::string s = "";
    if(rend & SDL_RENDERER_SOFTWARE)
    {
        s += "SOFTWARE ";
    }
    if(rend & SDL_RENDERER_ACCELERATED)
    {
        s += "ACCELERATED ";
    }
    if(rend & SDL_RENDERER_PRESENTVSYNC)
    {
        s += "PRESENTVSYNC ";
    }
    if(rend & SDL_RENDERER_TARGETTEXTURE)
    {
        s += "TARGETTEXTURE ";
    }
    if(rend & ~(SDL_RENDERER_TARGETTEXTURE + SDL_RENDERER_PRESENTVSYNC + SDL_RENDERER_ACCELERATED + SDL_RENDERER_SOFTWARE))
    {
        s += "UNKNOWN-FLAGS";
    }
    return s;
}

// --------------------------------------------------------------------------
std::string data_path = "";

#ifndef __EMSCRIPTEN__

void InitializeDataPath() {
    char *base_path = SDL_GetBasePath();
    if (base_path) {
        data_path = base_path;
    } else {
        data_path = SDL_strdup("./");
    }
}
#endif

int main(int argc, char *argv[]) {
    
#ifndef __EMSCRIPTEN__
    InitializeDataPath();
#endif
    
    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    printf("We compiled against SDL version %d.%d.%d ...\n",
           compiled.major, compiled.minor, compiled.patch);
    printf("But we are linking against SDL version %d.%d.%d.\n",
           linked.major, linked.minor, linked.patch);

    int posX = SDL_WINDOWPOS_UNDEFINED;
    int posY = SDL_WINDOWPOS_UNDEFINED;

    SDL_Init(SDL_INIT_EVERYTHING);

    // ---------
#if 1
    struct SDL_RendererInfo renderer_info;
    int num_r = SDL_GetNumRenderDrivers();
    printf("Number of renderers = %i\n", num_r);
    //int current_renderer = gulp_cpp.RendererInfo(gulp.app:get_SDL_renderer(), 0)
    //printf("Current Renderer: %s (%s)", current_renderer.name, render_flag_decode(current_renderer)) )
    for(int i = 0; i < num_r; i++)
    {
        int err = SDL_GetRenderDriverInfo(i, &renderer_info);
        if(err)
        {
            printf("Renderer %i: <ERROR> %s\n", i, SDL_GetError());
        }
        else
        {
            printf("Renderer %i: %s %s\n", i, renderer_info.name, render_flag_decode(renderer_info.flags).c_str());
        }
    }
#endif
    // --------
                
    SDL_Window *window = SDL_CreateWindow("Hello World", posX, posY, width, height, 0);
    // Check that the window was successfully created
    if (window == NULL) {
        // In the case that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }
    /*
        SDL_RENDERER_SOFTWARE - the renderer is a software fallback
        SDL_RENDERER_ACCELERATED - the renderer uses hardware acceleration
        SDL_RENDERER_PRESENTVSYNC - present is synchronized with the refresh rate
        SDL_RENDERER_TARGETTEXTURE - the renderer supports rendering to texture
     
     Returns a valid rendering context or NULL if there was an error; call SDL_GetError() for more information.
     */
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // Check that the renderer was successfully created
    if (renderer == NULL) {
        // In the case that the window could not be made...
        printf("Could not create renderer: %s\n", SDL_GetError());
        return 1;
    }
    int err = SDL_GetRendererInfo(renderer, &renderer_info);
    if(err)
    {
        printf("Current Renderer: <ERROR> %s\n", SDL_GetError());
    }
    else
    {
        printf("Current Renderer: %s %s\n", renderer_info.name, render_flag_decode(renderer_info.flags).c_str());
    }

    /*Returns a pointer to a new SDL_Surface structure or NULL if there was an error; call SDL_GetError() for more information. */
#if ENABLE_FOX
    SDL_Surface *bitmapSurface = SDL_LoadBMP((data_path+"fox.bmp").c_str());
#else
    SDL_Surface *bitmapSurface = SDL_LoadBMP((data_path+"icon.bmp").c_str();
#endif
    if (bitmapSurface == NULL) {
        // In the case that the window could not be made...
        printf("Could not create bitmapSurface: %s\n", SDL_GetError());
        return 1;
    }

#define PRINT_FIRST_FEW_PIXELS 0
#if PRINT_FIRST_FEW_PIXELS

    /* This is fast for surfaces that don't require locking. */
    /* Once locked, surface->pixels is safe to access. */
    SDL_LockSurface(bitmapSurface);

    /* This assumes that color value zero is black. Use
       SDL_MapRGBA() for more robust surface color mapping! */
    /* height times pitch is the size of the surface's whole buffer. */
    unsigned char* bytes = (unsigned char* )bitmapSurface->pixels;
    for(int i=0; i < 10; i++)
    {
        printf("%2x ", bytes[i]);
    }
    printf("\n");
    //SDL_memset(bitmapSurface->pixels, 0, surface->h * surface->pitch);

    SDL_UnlockSurface(bitmapSurface);
#endif
    
#if ENABLE_COLOUR_KEY
#if ENABLE_FOX
    int err1 = SDL_SetColorKey(bitmapSurface, 1,
                    SDL_MapRGB(bitmapSurface->format, 15, 127, 18));
#else
    /* set white to transparent on the happyface */
    int err1 = SDL_SetColorKey(bitmapSurface, 1,
                    SDL_MapRGB(bitmapSurface->format, 255, 255, 255));
#endif
    printf("Pixel Uint32 = %x\n", SDL_MapRGB(bitmapSurface->format, 15, 127, 18));

    if(err1)
    {
        //Returns 0 on success or a negative error code on failure; call SDL_GetError() for more information.
        printf("Could not enable colour key: %s\n", SDL_GetError());
        return 1;
    }
#endif
    
    //Returns the created texture or NULL on failure; call SDL_GetError() for more information.
    bitmapTex = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
    if (bitmapTex == NULL) {
        // In the case that the window could not be made...
        printf("Could not create bitmapTex: %s\n", SDL_GetError());
        return 1;
    }
    
#if ENABLE_BLEND_MODE
    int err2 = SDL_SetTextureBlendMode(bitmapTex, SDL_BLENDMODE_BLEND);
    if(err2)
    {
        //Returns 0 on success or a negative error code on failure; call SDL_GetError() for more information.
        printf("Could not enable blend mode: %s\n", SDL_GetError());
        return 1;
    }
#endif
    
    SDL_FreeSurface(bitmapSurface);


    initialize_sprites(renderer);

    printf("Starting main loop\n");
    
    // https://emscripten.org/docs/api_reference/emscripten.h.html#c.emscripten_set_main_loop
    // https://emscripten.org/docs/porting/emscripten-runtime-environment.html
    #ifdef __EMSCRIPTEN__
          // void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
#if USE_BROWSER_VYSNC
            emscripten_set_main_loop(one_iter, 0, 1);
#else
            emscripten_set_main_loop(one_iter, expected_framerate, 1);
#endif
    #else
          while (1) {
                one_iter();
                // Delay to keep frame rate constant (using SDL)
                //SDL_Delay(time_to_next_frame());
          }
    #endif
    
    SDL_DestroyTexture(bitmapTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}


