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
