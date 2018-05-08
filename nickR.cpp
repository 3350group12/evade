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

class Star {
public:
	Vec pos;
	int starx[20];
	int stary[20];
	struct Star *prev;
	struct Star *next;
public:
    Star(){
		for (int i=0; i<20; i++) {
		    starx[i] = rand()%100 - 50;
	    	stary[i] = rand()%100 - 50;
		}
    }
};


void nick_generate_starfield(Star* starfield, int xres, int yres, int starmax,
		int* stars)
{
	for (int i=0; i< starmax/2; i++) {
		Star *s = new Star;
		s->pos[0] = (Flt)(rand()%(xres));
		s->pos[1] = (Flt)(rand()%(yres));
		s->pos[2] = 0.0f;
		s->next = starfield;
		if (starfield != NULL) {
			starfield->prev = s;
		}
		*stars = *stars + 1;
		starfield = s;
	}
};

void nick_update_starfield(Star* starfield, int xres, int yres, int starmax,
		float velmax, int* stars)
{
	while (*stars < starmax) {
		Star *s = new Star;
		s->pos[0] = (Flt)(rand()%xres + xres);
		s->pos[1] = (Flt)(rand()%yres + yres) + 50;
		s->pos[2] = 0.0f;
		s->next = starfield;
		if (starfield != NULL) {
			starfield->prev = s;
		}
		*stars = *stars + 1;
		starfield = s;
	}
	Star *s = starfield;
	while (s) {
		s->pos[1] = s->pos[1]-(velmax/2);
		if (s->pos[1] < -50) {
			Star *saves = s->next;
			//
			if (s->prev == NULL) {
				if (s->next == NULL) {
					starfield = NULL;
				} else {
					s->next->prev = NULL;
					starfield = s->next;
				}
			} else {
				if (s->next == NULL) {
					s->prev->next = NULL;
				} else {
					s->prev->next = s->next;
					s->next->prev = s->prev;
				}
			}
			delete s;
			s = NULL;
			//
			if(starfield == NULL) {
				s = saves;
			} else {
				s = starfield;
			}
			*stars = *stars-1;
		}
	}
};

void nick_draw_starfield(Star* starfield)
{
	Star* s = starfield;
	while (s) {
		for (int i=0; i<20; i++) {
			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_POINTS);
			glVertex2f(s->pos[0],		s->pos[1]);
			glVertex2f(s->pos[0]-1.0f,	s->pos[1]);
			glVertex2f(s->pos[0]+1.0f,	s->pos[1]);
			glVertex2f(s->pos[0],		s->pos[1]-1.0f);
			glVertex2f(s->pos[0],		s->pos[1]+1.0f);
			glColor3f(0.8, 0.8, 0.8);
			glVertex2f(s->pos[0]-1.0f,	s->pos[1]-1.0f);
			glVertex2f(s->pos[0]-1.0f,	s->pos[1]+1.0f);
			glVertex2f(s->pos[0]+1.0f,	s->pos[1]-1.0f);
			glVertex2f(s->pos[0]+1.0f,	s->pos[1]+1.0f);
			
			glEnd();
		}
		s = s->next;
	}
};

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

void drawCircle(Flt radius,
		float r, float g, float b,
		float xc, float yc) {
	float x, y;
	int points = 30;
	glColor3f(r, g, b);
	glBegin(GL_TRIANGLE_FAN);
	for (int i=0; i<points; i++){
		x = radius * cos(i) + xc;
		y = radius * sin(i) + yc;
		glVertex3f (x, y, 0);

		x = radius * cos(i+0.3) + xc;
		y = radius * sin(i+0.3) + yc;
		glVertex3f (x, y, 0);
	}
	glEnd();
	glPopMatrix();
};

void nick_drawAsteroid(Flt radius,
		float r, float g, float b,
		float x, float y,
		float velx, float vely,
		float angle) {

	float circlex;
	float circley;
	float circler;
	float trailr;
	float trailg;
	float trailb;
	float bright;
//	int randfactor = 70;

	//trail 3
	bright = rand()%70;
	trailr = (255-(bright))/255;
	trailg = (200-(bright))/255;
	trailb = 0;
	circlex = x - radius * velx/7;
	circley = y - radius * vely/3.5;
	circler = radius/4;
	drawCircle(circler, trailr, trailg, trailb, circlex, circley);

	//trail 2
	bright = rand()%70;
	trailr = (255-(bright))/255;
	trailg = (165-(bright))/255;
	trailb = 0;
	circlex = x - radius * velx/10;
	circley = y - radius * vely/5;
	circler = radius/2;
	drawCircle(circler, trailr, trailg, trailb, circlex, circley);

	//trail 1
	bright = rand()%70;
	trailr = (255-(bright))/255;
	trailg = (100-(bright))/255;
	trailb = 0;
	circlex = x - radius * velx/17;
	circley = y - radius * vely/8.5;
	circler = 3*radius/4;
	drawCircle(circler, trailr, trailg, trailb, circlex, circley);

	//main circle
	drawCircle(radius+1, 0,0,0, x, y);
	drawCircle(radius, r, g, b, x, y);
	
	//crater 1
	circlex = radius/3 * cos(angle) + x;
	circley = radius/3 * sin(angle) + y;
	circler = radius/3;
	drawCircle(circler+1, r/4, g/4, b/4, circlex, circley);
	drawCircle(circler, r/4, g/4, b/4, circlex, circley);

	//crater 2
	circlex = 3*radius/5 * cos(angle + 2.5) + x;
	circley = 3*radius/5 * sin(angle + 2.5) + y;
	circler = radius/5;
	drawCircle(circler+1, r/4, g/4, b/4, circlex, circley);
	drawCircle(circler, r/4, g/4, b/4, circlex, circley);

	//crater 3
	circlex = 4*radius/6 * cos(angle + 4) + x;
	circley = 4*radius/6 * sin(angle + 4) + y;
	circler = radius/10;
	drawCircle(circler+1, r/4, g/4, b/4, circlex, circley);
	drawCircle(circler, r/4, g/4, b/4, circlex, circley);

};


