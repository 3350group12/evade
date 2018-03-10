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

using namespace std;

//Setup timers
const double OOBILLION = 1.0 / 1e9;
extern struct timespec timeStart, timeCurrent;
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
//--------------------------------------------------------------------

const uint64_t MAX  = 1 << 16 ;  

int jtL_Lab7()
{ 
    void use_cout( const uint64_t ) ; 
    void use_printf( const uint64_t ) ; 

    Rect r;
    r.bot = 200 ;
    r.left = 50;
    r.center = 0 ;
    ggprint8b( &r, 16, 0x00ffbbff, "JiannTyng Lu" );
    
    // Start Profiling: [ printf( u++ ) vs cout << u++ ] 
    use_printf ( MAX ) ;
    use_cout( MAX ) ; 
	return 0;
}

void use_printf( const uint64_t N ) 
{ 
    struct timespec start , end ;
    static double diff = 0.0 ; 
    clock_gettime( CLOCK_REALTIME, &start );
    // Timed 0
    for ( uint64_t u = 0 ; u < N ; )
        printf("%lu\n",u++) ;  
    clock_gettime( CLOCK_REALTIME, &end ) ; 
    diff += timeDiff( &start, &end );

    Rect r;
    r.bot = 185 ;
    r.left = 50;
    r.center = 0 ;
    ggprint8b( &r, 16, 0x00ffbbff, "timer_printf(): %lf", diff );
}

void use_cout( const uint64_t N ) 
{
    struct timespec start , end ;
    static double diff = 0.0 ; 
    clock_gettime( CLOCK_REALTIME, &start );
    // Timed 0
    for ( uint64_t u = 0 ; u < N ; )
        cout << u++ <<'\n' ; 
    clock_gettime( CLOCK_REALTIME, &end ) ; 
    diff += timeDiff( &start, &end );

    Rect r;
    r.bot = 170 ;
    r.left = 50;
    r.center = 0 ;
    ggprint8b( &r, 16, 0x00ffbbff, "timer__cout<<: %lf", diff );
}



