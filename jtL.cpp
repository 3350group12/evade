// Created by JiannTyng Lu
// Date 3.9.2018
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>

#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "fonts.h"
#include "Asteroid.h"

//Setup timers
const double OOBILLION = 1.0 / 1e9;
extern struct timespec timeStart, timeCurrent;
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
//--------------------------------------------------------------------





int jtL_Lab7()
{
    int divide_JT();
    int divide_opt_JT() ;

    Rect r;
    r.bot = 200 ;
    r.left = 50;
    r.center = 0 ;
    ggprint8b( &r, 16, 0x00ffbbff, "JiannTyng Lu" );
    divide_JT();
    divide_opt_JT();


	return 0;
}


int divide_JT( void ) 
{
	return 0;
}

int divide_opt_JT ( void )
{
	return 0;
} 




