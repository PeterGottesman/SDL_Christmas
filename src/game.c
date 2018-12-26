#include <unistd.h>
#include <math.h>
#include "game.h"
#include "window.h"

// Frames per rotation
#define PERIOD 100
#define OMEGA (2 * M_PI / PERIOD)

#define AMPLITUDE 100

// Microseconds waited per frame
// Denominator is a very rough FPS
#define USEC_PER_FRAME 1000000/120

#define TAIL_LENGTH 240
#define TREE_HEIGHT (HEIGHT/3)

typedef union RGBColor {
    struct RGB {
        uint8_t r,g,b,z;
    } RGB;
    uint32_t color;
} RGBColor;

struct node {
    SDL_Rect r;
    float cartX,cartY;
};

struct spiral {
    struct node *tail;
    RGBColor initialColor;
    double phi;
};

void gameloop(void)
{
    SDL_Surface *surf = SDL_GetWindowSurface(window);
    struct timespec before, after;
    int loopCount = 0;
    struct spiral spirals[2];
    spirals[0].initialColor.color = 0xff0000;
    spirals[0].tail = malloc(sizeof(struct node) * TAIL_LENGTH);
    spirals[1].initialColor.color = 0x00ff00;
    spirals[1].tail = malloc(sizeof(struct node) * TAIL_LENGTH);
    spirals[1].phi = M_PI;
    for (int i = 0; i < TAIL_LENGTH; ++i)
    {
	spirals[0].tail[i] = (struct node){(SDL_Rect){0, 0, 4, 4}, 0, 0};
	spirals[1].tail[i] = (struct node){(SDL_Rect){0, 0, 4, 4}, 0, 0};
;
    }
    int t = 0;
    if(clock_gettime(CLOCK_MONOTONIC, &before) == -1)
    {
	perror("clock failure");
	exit(-1);
    }
    while (1)
    {
	t++;
	usleep(USEC_PER_FRAME);
        SDL_Event e;
        if (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_Q]))
            {
                break;
            }
        }
        SDL_FillRect(surf,
                     NULL,
                     0x000000);

	for (int spiralInd = 0; spiralInd < 2; ++spiralInd)
	{
	    struct spiral sp = spirals[spiralInd];
	    float progress = 1 - (float)(t%(TREE_HEIGHT)) / TREE_HEIGHT;
	    float radius = AMPLITUDE * progress;
	    float cartX = (radius * cos(OMEGA * t + sp.phi));
	    float cartY = (radius * sin(OMEGA * t + sp.phi));

	    sp.tail[t%TAIL_LENGTH].cartX = cartX;
	    sp.tail[t%TAIL_LENGTH].cartY = cartY;
	
	    sp.tail[t%TAIL_LENGTH].r.x = cartX + WIDTH/2;
	    sp.tail[t%TAIL_LENGTH].r.y = HEIGHT - t%HEIGHT;

	    RGBColor rgb;
	    for (int i = 0; i < TAIL_LENGTH; ++i)
	    {
		rgb.color = sp.initialColor.color;
		
		// I meant to make this fade the background, but this looks better IMO
		rgb.RGB.r *= sp.tail[(t+i)%TAIL_LENGTH].cartY + AMPLITUDE;
		rgb.RGB.g *= sp.tail[(t+i)%TAIL_LENGTH].cartY + AMPLITUDE;
		rgb.RGB.b *= sp.tail[(t+i)%TAIL_LENGTH].cartY + AMPLITUDE;
		
		SDL_FillRect(surf, &sp.tail[(t+i)%TAIL_LENGTH].r, rgb.color);
	    }
	}

        SDL_UpdateWindowSurface(window);

	if(clock_gettime(CLOCK_MONOTONIC, &after) == -1)
	{
	    perror("clock failure");
	    exit(-1);
	}
	
	if(after.tv_sec > before.tv_sec)
	{
	    printf("%d frames this second\n", loopCount);
	    loopCount=0;
	    if(clock_gettime(CLOCK_MONOTONIC, &before) == -1)
	    {
		perror("clock failure");
		exit(-1);
	    }
	}
	else
	{
	  loopCount++;
	}
    }	
}

