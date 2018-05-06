//Nicholas Rosbrugh
//
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
using namespace std;
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "fonts.h"
#include "Asteroid.h"


const double oobillion = 1.0 / 1e9;
extern struct timespec timeStart, timeCurrent;
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);

//class Star {
//public:
//	Vec pos;
//	Vec vel;
//	float color[3];
//	struct Star *prev;
//	struct Star *next;
//public:
//	Star() {
//		prev = NULL;
//		next = NULL;
//	}
//};

void nick_Lab7()
{
    void nick_divide();
    void nick_divide_opt();

    Rect r;
    r.bot = 430;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 16, 0x0088bbff, "Nick Rosbrugh");
    nick_divide();
    nick_divide_opt();
}

void nick_divide()
{
    struct timespec start, end;
    static double diff = 0.0;
    clock_gettime(CLOCK_REALTIME, &start);
    //timer started
    
    long int a = 1125899906842624;
    int b = 2;
    for (int i=0; i<50; i++) {
		a = a/b;
    }

    //get and print time
    clock_gettime(CLOCK_REALTIME, &end);
    diff += timeDiff(&start, &end);
    Rect r;
    r.bot = 415;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 16, 0x0088bbff, "timer____: %lf", diff);
}

void nick_divide_opt()
{
    struct timespec start, end;
    static double diff = 0.0;
    clock_gettime(CLOCK_REALTIME, &start);
    //timer started
    
    long int a = 1125899906842624;
    for (int i=0; i<50; i++) {
		a >>= 1;
    }

    //get and print time
    clock_gettime(CLOCK_REALTIME, &end);
    diff += timeDiff(&start, &end);
    Rect r;
    r.bot = 400;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 16, 0x0088bbff, "timer_opt: %lf", diff);
}

int nick_dead(int shipXpos, int shipYpos, Asteroid* a)
{
	Flt d0, d1, dist;
	d0 = shipXpos - a->pos[0];
	d1 = shipYpos - a->pos[1];
	dist = (d0*d0 + d1*d1);
	if (dist < a->radius*a->radius) {
		return 1;
	}
	return 0;
}

void nick_explosion(float X, float Y)
{
    Flt xe,ye,r;
    glBegin(GL_LINES);
	for (int i=0; i<360; i++) {
		r = rnd()*40.0+20.0;
		xe = r*sin(i);
		ye = r*cos(i);
	    glColor3f(rnd()*.3+.7, rnd()*.3+.7, 0);
	    glVertex2f(X, Y);
	    glVertex2f(X+xe, Y+ye);
	}
	glEnd();
		
    return;
};

void nick_reset(int* dead, Asteroid *ahead, int* nasteroids)
{
	*dead = 0;
	Asteroid *node = ahead;
	while (node->next) {
		node = node->next;
		if (node->prev == NULL) {
			if (node->next == NULL) {
				ahead = NULL;
			} else {
				node->next->prev = NULL;
				ahead = node->next;
			}
		} else {
			if (node->next == NULL) {
				node->prev->next = NULL;
			} else {
				node->prev->next = node->next;
				node->next->prev = node->prev;
			}
		}
		delete node;
		node = ahead;
	}
	if (node) {
		node->pos[0] = -2000;
		node->pos[1] = -2000;
	}

	*nasteroids = 0;
};

void nick_drawContinue(int xLim, int yLim, int lives, int score)
{
	Rect r;
	r.centerx = xLim/2;
	r.centery = yLim/2;
	r.height = 45;
	r.width = 85;
	glColor3ub(100, 0, 20);
	glBegin(GL_QUADS);
		glVertex2i(r.centerx-r.width, r.centery-r.height);
		glVertex2i(r.centerx-r.width, r.centery+r.height);
		glVertex2i(r.centerx+r.width, r.centery+r.height);
		glVertex2i(r.centerx+r.width, r.centery-r.height);
	glEnd();
	glPopMatrix();

	r.centerx = xLim/2;
	r.centery = yLim/2;
	r.height = 40;
	r.width = 80;
	glColor3ub(20, 20, 20);
	glBegin(GL_QUADS);
		glVertex2i(r.centerx-r.width, r.centery-r.height);
		glVertex2i(r.centerx-r.width, r.centery+r.height);
		glVertex2i(r.centerx+r.width, r.centery+r.height);
		glVertex2i(r.centerx+r.width, r.centery-r.height);
    glEnd();

	if (lives)
	{
		r.bot = r.centery;
		r.left = r.centerx - 50;
		r.center = 0;
		ggprint16(&r, 28, 0x00ffdd00, "CONTINUE?");
		ggprint16(&r, 28, 0x00ffdd00, "   Y / N");
		glPopMatrix();
	} else {
		r.bot = r.centery;
		r.left = r.centerx - 50;
		r.center = 0;
		ggprint16(&r, 28, 0x00ffdd00, "GAME OVER");
		ggprint16(&r, 28, 0x00ffdd00, "Score: %i", score);
		glPopMatrix();
	}
};

void nick_score(int* score, int* asteroidlim, float* asteroidspd)
{
	*score = *score+1;
	if (*score%20 == 0) {
		*asteroidlim = *asteroidlim + 1;
	}
	if (*score%100 ==0) {
		*asteroidspd = *asteroidspd + 0.5;
	}
};
