/*Copyright 2017, 2018 Karl Landstrom <karl@miasap.se>

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.*/

#include ".obnc/Input.h"
#include "../../src/Util.h"
#include <obnc/OBNC.h>
#include <SDL/SDL.h>
#include <termios.h> /*POSIX*/
#include <unistd.h> /*POSIX*/
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

OBNC_LONGI int Input_TimeUnit_ = (OBNC_LONGI int) CLOCKS_PER_SEC;

static OBNC_LONGI int mouseLimitWidth = OBNC_INT_MAX;
static OBNC_LONGI int mouseLimitHeight = OBNC_INT_MAX;

static int IsAscii(const SDL_Event *event)
{
	Uint16 inputChar;
	int isAsciiChar, result;

	if (event->type == SDL_KEYDOWN) {
		inputChar = event->key.keysym.unicode;
		isAsciiChar = (inputChar & 0xFF80) == 0;
		result = (inputChar != '\0') && isAsciiChar;
	} else {
		result = 0;
	}
	return result;
}


OBNC_LONGI int Input_Available_(void)
{
	SDL_Event events[10];
	int count, result, i;

	result = 0;
	if (SDL_GetVideoSurface() != NULL) {
		SDL_PumpEvents();
		/*search the event queue for key down events*/
		count = SDL_PeepEvents(events, LEN(events), SDL_PEEKEVENT, SDL_EVENTMASK(SDL_KEYDOWN));
		if (count >= 0) {
			for (i = 0; i < count; i++) {
				if (IsAscii(&(events[i]))) {
					result++;
				}
			}
			if (result == 0) {
				/*remove events from the queue to allow new ones to enter*/
				count = SDL_PeepEvents(events, LEN(events), SDL_GETEVENT, (Uint32) -1);
				for (i = 0; i < count; i++) {
					if (events[i].type == SDL_QUIT) {
						exit(EXIT_SUCCESS);
					}
				}
			}
		} else {
			fprintf(stderr, "SDL_PeepEvents: %s\n", SDL_GetError());
		}
	} else {
		fprintf(stderr, "No display surface\n");
	}
	return result;
}


static void ReadGUI(char *ch)
{
	SDL_Event event;
	int done;

	do {
		done = SDL_WaitEvent(&event);
	} while (! done || ! IsAscii(&event));
	if (done) {
		*ch = (char) event.key.keysym.unicode;
	}
}


static void ReadCLI(char *ch)
{
	struct termios savedState, newState;
	int error, inputChar, isAscii;

	*ch = '\0';
	error = tcgetattr(STDIN_FILENO, &savedState);
	if (! error) {
		newState = savedState;
		newState.c_lflag &= (tcflag_t) ~(ECHO | ICANON);
		newState.c_cc[VMIN] = 1;
		error = tcsetattr(STDIN_FILENO, TCSANOW, &newState);
		if (! error) {
			do {
				inputChar = getchar();
				isAscii = (inputChar >= 0) && (inputChar < 128);
			} while (! isAscii);
			*ch = (char) inputChar;
			error = tcsetattr(STDIN_FILENO, TCSANOW, &savedState);
			if (error) {
				fprintf(stderr, "tcsetattr: %s\n", strerror(errno));
			}
		} else {
			fprintf(stderr, "tcsetattr: %s\n", strerror(errno));
		}
	} else {
		fprintf(stderr, "tcgetattr: %s\n", strerror(errno));
	}
}


void Input_Read_(char *ch)
{
	if (SDL_GetVideoSurface() != NULL) {
		ReadGUI(ch);
	} else {
		ReadCLI(ch);
	}
}


void Input_Mouse_(OBNC_LONGI unsigned int *keys, OBNC_LONGI int *x, OBNC_LONGI int *y)
{
	SDL_Surface *display;
	int x0, y0;
	Uint8 buttons;
	unsigned int leftPressed, middlePressed, rightPressed;

	display = SDL_GetVideoSurface();
	if (display != NULL) {
		SDL_PumpEvents();
		buttons = SDL_GetMouseState(&x0, &y0);
		*x = x0;
		*y = y0;
		*y = display->h - 1 - *y;
		if (*x >= mouseLimitWidth) {
			*x = mouseLimitWidth - 1;
		}
		if (*y >= mouseLimitHeight) {
			*y = mouseLimitHeight - 1;
		}
		leftPressed = (buttons & SDL_BUTTON_LMASK) != 0;
		middlePressed = (buttons & SDL_BUTTON_MMASK) != 0;
		rightPressed = (buttons & SDL_BUTTON_RMASK) != 0;

		*keys = (OBNC_LONGI unsigned int) (leftPressed << 2) | (middlePressed << 1) | rightPressed;
	} else {
		fprintf(stderr, "No display surface\n");
		exit(EXIT_FAILURE);
	}
}


void Input_SetMouseLimits_(OBNC_LONGI int w, OBNC_LONGI int h)
{
	assert(w > 0);
	assert(h > 0);
	
	mouseLimitWidth = w;
	mouseLimitHeight = h;
}


OBNC_LONGI int Input_Time_(void)
{
	return (OBNC_LONGI int) clock();
}


void Input_Init(void)
{
	/*do nothing*/
}
