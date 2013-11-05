/*
  Copyright (C) 2012-2013 Andreas Schiffler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "common.h"

#include "SDL2_gfxPrimitives.h"

static CommonState *state;

/* Screen size */
#define WIDTH	DEFAULT_WINDOW_WIDTH
#define HEIGHT	DEFAULT_WINDOW_HEIGHT

/* viewport border */
#define BORDER	10

#define NUM_RANDOM	4096

/* Coordinates */
static Sint16 rx[NUM_RANDOM], rx[NUM_RANDOM], ry[NUM_RANDOM], ry[NUM_RANDOM];

/* Triangles */
static Sint16 tx1[NUM_RANDOM][3], tx1[NUM_RANDOM][3], ty1[NUM_RANDOM][3], ty1[NUM_RANDOM][3];

/* Squares (made of 2 triangles) */
static Sint16 sx1[NUM_RANDOM][6], sx1[NUM_RANDOM][6], sy1[NUM_RANDOM][6], sy1[NUM_RANDOM][6];

/* Line widths */
static Uint8 lw[NUM_RANDOM];

/* Radii and offsets */
static Sint16 rr1[NUM_RANDOM], rr2[NUM_RANDOM];

/* Start and stop angles */
static Sint16 a1[NUM_RANDOM], a2[NUM_RANDOM];

/* RGB colors and alpha */
static char rr[NUM_RANDOM], rg[NUM_RANDOM], rb[NUM_RANDOM], ra[NUM_RANDOM];

/*! 
\brief Generate an array of random screen coordinates, radii and RGBA values with an offset for drawing tests.

\param seed The seed for random number generation. If negative, uses current time as seed.
*/
void InitRandomPoints(int seed)
{
	int i;
	float af;

	/* Initialize random number generator */
	if (seed < 0) {
      srand((unsigned int)time(NULL));
	} else {
      srand((unsigned int)seed);
	}

	for (i=0; i<NUM_RANDOM; i++) {
		/* Random points in a quadrant */
		rx[i]=rand() % (WIDTH/2);
		ry[i]=rand() % (HEIGHT/2);

		/* 5-Pixel Triangle */
		tx1[i][0]=rx[i];
		ty1[i][0]=ry[i];
		tx1[i][1]=rx[i]+1;
		ty1[i][1]=ry[i]+2;
		tx1[i][2]=rx[i]+2;
		ty1[i][2]=ry[i]+1;

		/* 10x10 square made from 3 triangles */
		sx1[i][0]=rx[i];
		sy1[i][0]=ry[i];
		sx1[i][1]=rx[i]+10;
		sy1[i][1]=ry[i];
		sx1[i][2]=rx[i];
		sy1[i][2]=ry[i]+10;
		sx1[i][3]=rx[i];
		sy1[i][3]=ry[i]+10;
		sx1[i][4]=rx[i]+10;
		sy1[i][4]=ry[i];
		sx1[i][5]=rx[i]+10;
		sy1[i][5]=ry[i]+10;

		/* Line widths */
		lw[i]=2 + (rand() % 7);

		/* Random Radii */
		rr1[i]=rand() % 32;
		rr2[i]=rand() % 32;

		/* Random Angles */
		a1[i]=rand() % 360;
		a2[i]=rand() % 360;

		/* Random Colors */
		rr[i]=rand() & 255;
		rg[i]=rand() & 255;
		rb[i]=rand() & 255;

		/* X-position dependent Alpha */
		af=((float)rx[i]/(float)(WIDTH/2));
		ra[i]=(int)(255.0*af);
	} 
}

/* Set a viewport based on a rect with a border */
void SetViewport(SDL_Renderer *renderer, int x1, int y1, int x2, int y2)
{ 
	SDL_Rect clip;
	clip.x = x1+BORDER;
	clip.y = y1+BORDER;
	clip.w = x2-x1-2*BORDER;
	clip.h = y2-y1-2*BORDER;
	SDL_RenderSetViewport(renderer, &clip);
}

/* Set a viewport rectangle based on a rect */
void SetViewportNoBorder(SDL_Renderer *renderer, int x1, int y1, int x2, int y2)
{ 
	SDL_Rect clip;
	clip.x = x1;
	clip.y = y1;
	clip.w = x2-x1;
	clip.h = y2-y1;
	SDL_RenderSetViewport(renderer, &clip);
}

/* Clear currently set viewport rectangle (if any) */
void ClearViewport(SDL_Renderer *renderer)
{ 
	SDL_RenderSetViewport(renderer, NULL);
}

#define TLEN 256

/* Clear the screen, draw guides and color areas and titles */
void ClearScreen(SDL_Renderer *renderer, const char *title)
{
	int x,y;
	float stepx, stepy, fx, fy, fxy;
	char titletext[TLEN+1];

	/* Clear the screen */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

	/* Draw the colored area */
	stepx=1.0f/(WIDTH/2);
	stepy=1.0f/((HEIGHT-80)/2);
	fx=0.0;
	for (x=WIDTH/2; x<WIDTH; x++) {
		fy=0.0;
		for (y=(HEIGHT-40)/2+60; y<HEIGHT; y++) {
			fxy=1.0f-fx*fy;
			pixelRGBA(renderer,x,y,(int)(128.0*fx*fx),(int)(128.0*fxy*fxy),(int)(128.0*fy*fy),255);
			fy += stepy;
		}
		fx += stepx;
	}

	/* Segment and titles */
	hlineRGBA(renderer, 0, WIDTH, 40-1, 255,255,255,255);
	hlineRGBA(renderer, 0, WIDTH, 60-1, 255,255,255,255);
	hlineRGBA(renderer, 0, WIDTH, (HEIGHT-40)/2+40+1, 255,255,255,255); 
	hlineRGBA(renderer, 0, WIDTH, (HEIGHT-40)/2+60-1, 255,255,255,255); 
	vlineRGBA(renderer, WIDTH/2, 40, HEIGHT, 255,255,255,255);
	SDL_strlcpy(titletext,"Current Primitive: ",TLEN);
	SDL_strlcat(titletext,title,TLEN);
	SDL_strlcat(titletext,"  -  Space to continue. ESC to Quit.",TLEN);
	stringRGBA (renderer, WIDTH/2-4*strlen(titletext),10-4,titletext,255,255,0,255);
	SDL_strlcpy(titletext,"A=255 on Black",TLEN);
	stringRGBA (renderer, WIDTH/4-4*strlen(titletext),50-4,titletext,255,255,255,255);
	SDL_strlcpy(titletext,"A=0-254 on Black",TLEN);
	stringRGBA (renderer, 3*WIDTH/4-4*strlen(titletext),50-4,titletext,255,255,255,255);
	SDL_strlcpy(titletext,"A=255, Color Test",TLEN);
	stringRGBA (renderer, WIDTH/4-4*strlen(titletext),(HEIGHT-40)/2+50-4,titletext,255,255,255,255);
	SDL_strlcpy(titletext,"A=0-254 on Color",TLEN);
	stringRGBA (renderer, 3*WIDTH/4-4*strlen(titletext),(HEIGHT-40)/2+50-4,titletext,255,255,255,255);
}

/* !< Function pointer to a primitives test function */
typedef int (*PrimitivesTestCaseFp)(SDL_Renderer *renderer);

void ExecuteTest(SDL_Renderer *renderer, PrimitivesTestCaseFp testCase, const char * testName)
{
	char titletext[TLEN+1];
    Uint32 then, now, numPrimitives;
	ClearScreen(renderer, testName);
	then = SDL_GetTicks();
	numPrimitives = testCase(renderer);
	now = SDL_GetTicks();
    if (now > then) {
        double fps = ((double) numPrimitives * 1000) / (now - then);
        SDL_snprintf(titletext, TLEN, "%20s: %10.1f /sec", testName, fps);
	    stringRGBA (renderer, WIDTH/2-4*strlen(titletext),30-4,titletext,255,255,255,255);
		SDL_Log(titletext);
    }
}

/* --------------------- Tests ------------------------ */

int TestPixel(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 1;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		pixelRGBA(renderer, rx[i], ry[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		pixelRGBA(renderer, rx[i], ry[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		pixelRGBA(renderer, rx[i], ry[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		pixelRGBA(renderer, rx[i], ry[i], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);
	
	return (4 * NUM_RANDOM) / step;
}

int TestHline(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 2;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		hlineRGBA(renderer, rx[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		hlineRGBA(renderer, rx[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		hlineRGBA(renderer, rx[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		hlineRGBA(renderer, rx[i], rx[i]+rr1[i], ry[i+1], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestVline(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 2;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		vlineRGBA(renderer, rx[i], ry[i], ry[i+1], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		vlineRGBA(renderer, rx[i], ry[i], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		vlineRGBA(renderer, rx[i], ry[i], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		vlineRGBA(renderer, rx[i], ry[i], ry[i]+rr1[i], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestRectangle(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 2;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		rectangleRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		rectangleRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		rectangleRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		rectangleRGBA(renderer, rx[i], ry[i], rx[i]+rr1[i], ry[i]+rr2[i], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestRoundedRectangle(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 2;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		roundedRectangleRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], 4, rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		roundedRectangleRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], 4, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		roundedRectangleRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], 4, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		roundedRectangleRGBA(renderer, rx[i], ry[i], rx[i]+rr1[i], ry[i]+rr2[i], 4, r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestBox(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 2;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		boxRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		boxRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		boxRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		boxRGBA(renderer, rx[i], ry[i], rx[i]+rr1[i], ry[i]+rr2[i], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestLine(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 2;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		lineRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		lineRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		lineRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		lineRGBA(renderer, rx[i], ry[i], rx[i]+rr1[i], ry[i]+rr2[i], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestCircle(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 2;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		circleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		circleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		circleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		circleRGBA(renderer, rx[i], ry[i], rr1[i], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestAACircle(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 2;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		aacircleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		aacircleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		aacircleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		aacircleRGBA(renderer, rx[i], ry[i], rr1[i], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestFilledCircle(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 2;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		filledCircleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		filledCircleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		filledCircleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		filledCircleRGBA(renderer, rx[i], ry[i], rr1[i], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestEllipse(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 2;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		ellipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		ellipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		ellipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		ellipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestAAEllipse(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 2;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		aaellipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		aaellipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		aaellipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		aaellipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestFilledEllipse(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 2;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		filledEllipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		filledEllipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		filledEllipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		filledEllipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestBezier(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 5;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		bezierRGBA(renderer, &rx[i], &ry[i], 3, 100, rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		bezierRGBA(renderer, &rx[i], &ry[i], 3, 100, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		bezierRGBA(renderer, &rx[i], &ry[i], 3, 100, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		rx[i+1]=rx[i]+rr1[i];
		rx[i+2]=rx[i];
		ry[i+1]=ry[i];
		ry[i+2]=ry[i]+rr2[i];
		bezierRGBA(renderer, &rx[i], &ry[i], 3, 100, r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestPolygon(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 3;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		polygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		polygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		polygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		rx[i+1]=rx[i]+rr1[i];
		rx[i+2]=rx[i];
		ry[i+1]=ry[i];
		ry[i+2]=ry[i]+rr2[i];
		polygonRGBA(renderer, &rx[i], &ry[i], 3, r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestAAPolygon(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 4;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		aapolygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		aapolygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		aapolygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		rx[i+1]=rx[i]+rr1[i];
		rx[i+2]=rx[i];
		ry[i+1]=ry[i];
		ry[i+2]=ry[i]+rr2[i];
		aapolygonRGBA(renderer, &rx[i], &ry[i], 3, r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestFilledPolygon(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 4;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		filledPolygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		filledPolygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		filledPolygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		rx[i+1]=rx[i]+rr1[i];
		rx[i+2]=rx[i];
		ry[i+1]=ry[i];
		ry[i+2]=ry[i]+rr2[i];
		filledPolygonRGBA(renderer, &rx[i], &ry[i], 3, r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestTrigon(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 1;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		trigonRGBA(renderer, tx1[i][0], ty1[i][0], tx1[i][1], ty1[i][1], tx1[i][2], ty1[i][2], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		trigonRGBA(renderer, tx1[i][0], ty1[i][0], tx1[i][1], ty1[i][1], tx1[i][2], ty1[i][2], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		trigonRGBA(renderer, tx1[i][0], ty1[i][0], tx1[i][1], ty1[i][1], tx1[i][2], ty1[i][2], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		trigonRGBA(renderer, tx1[i][0], ty1[i][0], tx1[i][1], ty1[i][1], tx1[i][2], ty1[i][2], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestArc(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 1;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		arcRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		arcRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		arcRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		arcRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestPie(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 1;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		pieRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		pieRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		pieRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		pieRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestFilledPie(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 1;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		filledPieRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		filledPieRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		filledPieRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		filledPieRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

int TestThickLine(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;
	int step = 6;

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		thickLineRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], lw[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		thickLineRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], lw[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		thickLineRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], lw[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			r=255; g=0; b=0; 
		} else if (rx[i] < (WIDTH/3) ) {
			r=0; g=255; b=0; 
		} else {
			r=0; g=0; b=255; 
		}
		thickLineRGBA(renderer, rx[i], ry[i], rx[i]+rr1[i], ry[i]+rr2[i], lw[i], r, g, b, 255);
	}

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}


int TestTexturedPolygon(SDL_Renderer *renderer)
{
	int i;
	Uint32 color;
	SDL_Surface *texture;
	int step = 1;

	/* Create texture */
	texture = SDL_CreateRGBSurface(SDL_SWSURFACE,
		2, 2, 32,
		0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	/* Draw A=255 */
	((Uint32 *)texture->pixels)[0] = 0xffffffff;
	((Uint32 *)texture->pixels)[1] = 0xffff00ff;
	((Uint32 *)texture->pixels)[2] = 0x00ffffff;
	((Uint32 *)texture->pixels)[3] = 0xff00ffff;
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		texturedPolygon(renderer, &tx1[i][0], &ty1[i][0], 3, texture, 0, 0);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += step) {
		((Uint32 *)texture->pixels)[0] = 0xffffff00 | ra[i];
		((Uint32 *)texture->pixels)[1] = 0xffff0000 | ra[i];
		((Uint32 *)texture->pixels)[2] = 0x00ffff00 | ra[i];
		((Uint32 *)texture->pixels)[3] = 0xff00ff00 | ra[i];
		texturedPolygon(renderer, &tx1[i][0], &ty1[i][0], 3, texture, 0, 0);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		((Uint32 *)texture->pixels)[0] = 0xffffff00 | ra[i];
		((Uint32 *)texture->pixels)[1] = 0xffff0000 | ra[i];
		((Uint32 *)texture->pixels)[2] = 0x00ffff00 | ra[i];
		((Uint32 *)texture->pixels)[3] = 0xff00ff00 | ra[i];
		texturedPolygon(renderer, &tx1[i][0], &ty1[i][0], 3, texture, 0, 0);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += step) {
		if (rx[i] < (WIDTH/6))  {
			color = 0xff0000ff;
		} else if (rx[i] < (WIDTH/3) ) {
			color = 0x00ff00ff;
		} else {
			color = 0x0000ffff;
		}
		((Uint32 *)texture->pixels)[0] = color;
		((Uint32 *)texture->pixels)[1] = color;
		((Uint32 *)texture->pixels)[2] = color;
		((Uint32 *)texture->pixels)[3] = color;

		texturedPolygon(renderer, &tx1[i][0], &ty1[i][0], 3, texture, 0, 0);
	}

	SDL_FreeSurface(texture);

	/* Clear viewport */
	ClearViewport(renderer);

	return (4 * NUM_RANDOM) / step;
}

/* ====== Main */

int main(int argc, char *argv[])
{
    int i, done, drawn, test;
    SDL_Event event;
    Uint32 then, now, frames;
	int numTests;

    /* Initialize test framework */
    state = CommonCreateState(argv, SDL_INIT_VIDEO);
    if (!state) {
        return 1;
    }

	SDL_Log("SDL2_gfx %i.%i.%i: testgfx", SDL2_GFXPRIMITIVES_MAJOR, SDL2_GFXPRIMITIVES_MINOR, SDL2_GFXPRIMITIVES_MICRO);
	SDL_Log("Platform: %s", SDL_GetPlatform());

    for (i = 1; i < argc;) {
        int consumed;

        consumed = CommonArg(state, i);
        if (consumed < 0) {
            fprintf(stderr,
                    "Usage: %s %s\n",
                    argv[0], CommonUsage(state));
            return 1;
        }
        i += consumed;
    }
    if (!CommonInit(state)) {
        return 2;
    }

    /* Create the windows and initialize the renderers */
    for (i = 0; i < state->num_windows; ++i) {
        SDL_Renderer *renderer = state->renderers[i];
        SDL_RendererInfo info;
        SDL_GetRendererInfo(state->renderers[i], &info);
		SDL_Log("Renderer %i: %s %s", i, info.name, (info.flags | SDL_RENDERER_ACCELERATED) ? "(Accelerated)" : "");
        SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
        SDL_RenderClear(renderer);
    }

    /* Main render loop */
    frames = 0;
    then = SDL_GetTicks();
    done = 0;
	drawn = 0;
	test = 0;
    while (!done) {
        /* Check for events */
        ++frames;
        while (SDL_PollEvent(&event)) {
            CommonEvent(state, &event, &done);
			switch (event.type) {
				case SDL_KEYDOWN: {
					switch (event.key.keysym.sym) {
						case SDLK_SPACE: {
							/* Switch to next test */
							test++;
							drawn = 0;
							break;
						}
					}
					break;
				}
				case SDL_MOUSEBUTTONDOWN: {
					switch (event.button.button) {
						case SDL_BUTTON_LEFT: {
							/* Switch to next test */
							test++;
							drawn = 0;
							break;
						}
						case SDL_BUTTON_RIGHT: {
							/* Switch to prev test */
							test--;
							drawn = 0;
							break;
						}
					}
					break;
				}
			}
        }

		if (!drawn) {
			/* Set test range */
			numTests = 22;
			if (test<0) { 
				test = (numTests - 1); 
			} else {
				test = test % (numTests + 1);
			}

			/* Create random points */
			InitRandomPoints(test);

			/* Draw */
			for (i = 0; i < state->num_windows; ++i) {
				SDL_Renderer *renderer = state->renderers[i];
               
				switch (test) {
					case 0: {
						ExecuteTest(renderer, TestPixel, "Pixel");
						break;
					}
					case 1: {
						ExecuteTest(renderer, TestHline, "Hline");
						break;
					}
					case 2: {
						ExecuteTest(renderer, TestVline, "Vline");
						break;
					}
					case 3: {
						ExecuteTest(renderer, TestRectangle, "Rectangle");
						break;
					}
					case 4: {
						ExecuteTest(renderer, TestRoundedRectangle, "RoundedRectangle");
						break;
					}
					case 5: {
						ExecuteTest(renderer, TestBox, "Box");
						break;
					}
					case 6: {
						ExecuteTest(renderer, TestLine, "Line");
						break;
					}
					case 7: {
						ExecuteTest(renderer, TestCircle, "Circle");
						break;
					}
					case 8: {
						ExecuteTest(renderer, TestAACircle, "AACircle");
						break;
					}
					case 9: {
						ExecuteTest(renderer, TestFilledCircle, "FilledCircle");
						break;
					}
					case 10: {
						ExecuteTest(renderer, TestEllipse, "Ellipse");
						break;
					}
					case 11: {
						ExecuteTest(renderer, TestAAEllipse, "AAEllipse");
						break;
					}
					case 12: {
						ExecuteTest(renderer, TestFilledEllipse, "FilledEllipse");
						break;
					}
					case 13: {
						ExecuteTest(renderer, TestBezier, "Bezier");
						break;
					}
					case 14: {
						ExecuteTest(renderer, TestPolygon, "Polygon");
						break;
					}
					case 15: {
						ExecuteTest(renderer, TestAAPolygon, "AAPolygon");
						break;
					}
					case 16: {
						ExecuteTest(renderer, TestFilledPolygon, "FilledPolygon");
						break;
					}
					case 17: {
						ExecuteTest(renderer, TestTrigon, "Trigon");
						break;
					}
					case 18: {
						ExecuteTest(renderer, TestArc, "Arc");
						break;
					}
					case 19: {
						ExecuteTest(renderer, TestPie, "Pie");
						break;
					}
					case 20: {
						ExecuteTest(renderer, TestFilledPie, "FilledPie");
						break;
					}
					case 21: {
						ExecuteTest(renderer, TestThickLine, "ThickLine");
						break;
					}
					case 22: {
						ExecuteTest(renderer, TestTexturedPolygon, "TexturedPolygon");
						break;
					}
					default: {
						ClearScreen(renderer, "Unknown Test");
						break;
					}
				}

				SDL_RenderPresent(renderer);
			}
			drawn = 1;
		}

		/* Adjust framerate */
		SDL_Delay(25);
    }

    CommonQuit(state);

    /* Print out some timing information */
    now = SDL_GetTicks();
    if (now > then) {
        double fps = ((double) frames * 1000) / (now - then);
        printf("%2.2f frames per second\n", fps);
    }
    return 0;
}
