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

#define NUM_RANDOM	512

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

/* Generate an array of random screen coordinates and RGBA values with an offset */
void InitRandomPoints()
{
	int i;
	float af;

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

/* Clear the screen, draw guides and color areas and titles */
void ClearScreen(SDL_Renderer *renderer, char *title)
{
	int x,y;
	float stepx, stepy, fx, fy, fxy;
	char titletext[257];

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
	hlineRGBA(renderer, 0, WIDTH, 20-1, 255,255,255,255);
	hlineRGBA(renderer, 0, WIDTH, 60-1, 255,255,255,255);
	hlineRGBA(renderer, 0, WIDTH, (HEIGHT-40)/2+40+1, 255,255,255,255); 
	hlineRGBA(renderer, 0, WIDTH, (HEIGHT-40)/2+60-1, 255,255,255,255); 
	vlineRGBA(renderer, WIDTH/2, 20, HEIGHT, 255,255,255,255);
	strncpy(titletext,"Current Primitive: ",256);
	strncat(titletext,title,256);
	strncat(titletext,"  -  Space to continue. ESC to Quit.",256);
	stringRGBA (renderer, WIDTH/2-4*strlen(titletext),10-4,titletext,255,255,0,255);
	strncpy(titletext,"A=255 on Black",256);
	stringRGBA (renderer, WIDTH/4-4*strlen(titletext),50-4,titletext,255,255,255,255);
	strncpy(titletext,"A=0-254 on Black",256);
	stringRGBA (renderer, 3*WIDTH/4-4*strlen(titletext),50-4,titletext,255,255,255,255);
	strncpy(titletext,"A=255, Color Test",256);
	stringRGBA (renderer, WIDTH/4-4*strlen(titletext),(HEIGHT-40)/2+50-4,titletext,255,255,255,255);
	strncpy(titletext,"A=0-254 on Color",256);
	stringRGBA (renderer, 3*WIDTH/4-4*strlen(titletext),(HEIGHT-40)/2+50-4,titletext,255,255,255,255);
}

void TestPixel(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		pixelRGBA(renderer, rx[i], ry[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		pixelRGBA(renderer, rx[i], ry[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
		pixelRGBA(renderer, rx[i], ry[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
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
}

void TestHline(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += 2) {
		hlineRGBA(renderer, rx[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += 2) {
		hlineRGBA(renderer, rx[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += 2) {
		hlineRGBA(renderer, rx[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += 2) {
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
}

void TestVline(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += 2) {
		vlineRGBA(renderer, rx[i], ry[i], ry[i+1], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += 2) {
		vlineRGBA(renderer, rx[i], ry[i], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += 2) {
		vlineRGBA(renderer, rx[i], ry[i], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += 2) {
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
}

void TestRectangle(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += 2) {
		rectangleRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += 2) {
		rectangleRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += 2) {
		rectangleRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += 2) {
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
}

void TestRoundedRectangle(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += 2) {
		roundedRectangleRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], 4, rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += 2) {
		roundedRectangleRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], 4, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += 2) {
		roundedRectangleRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], 4, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += 2) {
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
}

void TestBox(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += 2) {
		boxRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += 2) {
		boxRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += 2) {
		boxRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += 2) {
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
}

void TestLine(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += 2) {
		lineRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += 2) {
		lineRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += 2) {
		lineRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += 2) {
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
}

void TestCircle(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		circleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		circleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
		circleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
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
}

void TestAACircle(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		aacircleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		aacircleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
		aacircleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
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
}

void TestFilledCircle(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		filledCircleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		filledCircleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
		filledCircleRGBA(renderer, rx[i], ry[i], rr1[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
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
}

void TestEllipse(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		ellipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		ellipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
		ellipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
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
}

void TestAAEllipse(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		aaellipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		aaellipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
		aaellipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
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
}

void TestFilledEllipse(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		filledEllipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		filledEllipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
		filledEllipseRGBA(renderer, rx[i], ry[i], rr1[i], rr2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
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
}

void TestBezier(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
		bezierRGBA(renderer, &rx[i], &ry[i], 3, 100, rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
		bezierRGBA(renderer, &rx[i], &ry[i], 3, 100, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
		bezierRGBA(renderer, &rx[i], &ry[i], 3, 100, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
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
}

void TestPolygon(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
		polygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
		polygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
		polygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
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
}

void TestAAPolygon(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
		aapolygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
		aapolygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
		aapolygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
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
}

void TestFilledPolygon(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
		filledPolygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
		filledPolygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
		filledPolygonRGBA(renderer, &rx[i], &ry[i], 3, rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<(NUM_RANDOM-3); i += 3) {
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
}

void TestTrigon(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		trigonRGBA(renderer, tx1[i][0], ty1[i][0], tx1[i][1], ty1[i][1], tx1[i][2], ty1[i][2], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		trigonRGBA(renderer, tx1[i][0], ty1[i][0], tx1[i][1], ty1[i][1], tx1[i][2], ty1[i][2], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
		trigonRGBA(renderer, tx1[i][0], ty1[i][0], tx1[i][1], ty1[i][1], tx1[i][2], ty1[i][2], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
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
}

void TestArc(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		arcRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		arcRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
		arcRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
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
}

void TestPie(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		pieRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		pieRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
		pieRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
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
}

void TestFilledPie(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		filledPieRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		filledPieRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
		filledPieRGBA(renderer, rx[i], ry[i], rr1[i], a1[i], a2[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
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
}

void TestThickLine(SDL_Renderer *renderer)
{
	int i;
	char r,g,b;

	/* Create random points */
	InitRandomPoints();

	/* Draw A=255 */
	SetViewport(renderer,0,60,WIDTH/2,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += 5) {
		thickLineRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], lw[i], rr[i], rg[i], rb[i], 255);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i += 5) {
		thickLineRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], lw[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += 5) {
		thickLineRGBA(renderer, rx[i], ry[i], rx[i+1], ry[i+1], lw[i], rr[i], rg[i], rb[i], ra[i]);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i += 5) {
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
}


void TestTexturedPolygon(SDL_Renderer *renderer)
{
	int i;
	Uint32 color;
	SDL_Surface *texture;

	/* Create random points */
	InitRandomPoints();

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
	for (i=0; i<NUM_RANDOM; i++) {
		texturedPolygon(renderer, &tx1[i][0], &ty1[i][0], 3, texture, 0, 0);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,60,WIDTH,60+(HEIGHT-80)/2);
	for (i=0; i<NUM_RANDOM; i++) {
		((Uint32 *)texture->pixels)[0] = 0xffffff00 | ra[i];
		((Uint32 *)texture->pixels)[1] = 0xffff0000 | ra[i];
		((Uint32 *)texture->pixels)[2] = 0x00ffff00 | ra[i];
		((Uint32 *)texture->pixels)[3] = 0xff00ff00 | ra[i];
		texturedPolygon(renderer, &tx1[i][0], &ty1[i][0], 3, texture, 0, 0);
	}

	/* Draw A=various */
	SetViewport(renderer,WIDTH/2,80+(HEIGHT-80)/2,WIDTH,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
		((Uint32 *)texture->pixels)[0] = 0xffffff00 | ra[i];
		((Uint32 *)texture->pixels)[1] = 0xffff0000 | ra[i];
		((Uint32 *)texture->pixels)[2] = 0x00ffff00 | ra[i];
		((Uint32 *)texture->pixels)[3] = 0xff00ff00 | ra[i];
		texturedPolygon(renderer, &tx1[i][0], &ty1[i][0], 3, texture, 0, 0);
	}

	/* Draw Colortest */
	SetViewport(renderer,0,80+(HEIGHT-80)/2,WIDTH/2,HEIGHT);
	for (i=0; i<NUM_RANDOM; i++) {
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
        SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
        SDL_RenderClear(renderer);
    }

	/* Initialize random number generator */
    srand((unsigned int)time(NULL));

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

			/* Draw */
			for (i = 0; i < state->num_windows; ++i) {
				SDL_Renderer *renderer = state->renderers[i];

				switch (test) {
					case 0: {
						ClearScreen(renderer, "Pixel");
						TestPixel(renderer); 
						break;
					}
					case 1: {
						ClearScreen(renderer, "Hline");
						TestHline(renderer);
						break;
					}
					case 2: {
						ClearScreen(renderer, "Vline");
						TestVline(renderer);
						break;
					}
					case 3: {
						ClearScreen(renderer, "Rectangle");
						TestRectangle(renderer);
						break;
					}
					case 4: {
						ClearScreen(renderer, "RoundedRectangle");
						TestRoundedRectangle(renderer);
						break;
					}
					case 5: {
						ClearScreen(renderer, "Box");
						TestBox(renderer);
						break;
					}
					case 6: {
						ClearScreen(renderer, "Line");
						TestLine(renderer);
						break;
					}
					case 7: {
						ClearScreen(renderer, "Circle");
						TestCircle(renderer);
						break;
					}
					case 8: {
						ClearScreen(renderer, "AACircle");
						TestAACircle(renderer);
						break;
					}
					case 9: {
						ClearScreen(renderer, "FilledCircle");
						TestFilledCircle(renderer);
						break;
					}
					case 10: {
						ClearScreen(renderer, "Ellipse");
						TestEllipse(renderer);
						break;
					}
					case 11: {
						ClearScreen(renderer, "AAEllipse");
						TestAAEllipse(renderer);
						break;
					}
					case 12: {
						ClearScreen(renderer, "FilledEllipse");
						TestFilledEllipse(renderer);
						break;
					}
					case 13: {
						ClearScreen(renderer, "Bezier");
						TestBezier(renderer);
						break;
					}
					case 14: {
						ClearScreen(renderer, "Polygon");
						TestPolygon(renderer);
						break;
					}
					case 15: {
						ClearScreen(renderer, "AAPolygon");
						TestAAPolygon(renderer);
						break;
					}
					case 16: {
						ClearScreen(renderer, "Polygon");
						TestFilledPolygon(renderer);
						break;
					}
					case 17: {
						ClearScreen(renderer, "Trigon");
						TestTrigon(renderer);
						break;
					}
					case 18: {
						ClearScreen(renderer, "Arc");
						TestArc(renderer);
						break;
					}
					case 19: {
						ClearScreen(renderer, "Pie");
						TestPie(renderer);
						break;
					}
					case 20: {
						ClearScreen(renderer, "FilledPie");
						TestFilledPie(renderer);
						break;
					}
					case 21: {
						ClearScreen(renderer, "ThickLine");
						TestThickLine(renderer);
						break;
					}
					case 22: {
						ClearScreen(renderer, "TexturedPolygon");
						TestTexturedPolygon(renderer);
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
