
//Modified by: Antonio Solorio, Nicholas Rosbrugh 
//program: asteroids.cpp
//author:  Gordon Griesel
//date:    2014 - 2018
//mod spring 2015: added constructors
//mod spring 2018: X11 wrapper class
//This program is a game starting point for a 3350 project.
//
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <stdlib.h>
using namespace std;
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "fonts.h"
#include "Asteroid.h"
#include "Bullet.h"
#include "antonioS.h"

//constants
const int MENU = 0;
const int GAME = 1;
//const int
int program_state = GAME;

const float timeslice = 1.0f;
const float gravity = -0.2f;
const float SHIPSPEED = 10;
#define PI 3.141592653589793
#define ALPHA 1
const int SUPER = 100000;
const int SUPERSIZE = (sqrt(SUPER)/20 +20);
const int MAX_BULLETS = 1111;
const int MAX_POWERUPS = 4;
const int BULLETSPEED = 30;
const int POWERUP_RATE = 5;
const Flt MINIMUM_ASTEROID_SIZE = 30.0;
int GODMODE = 0;
//-----------------------------------------------------------------------------
//Setup timers
const double oobillion = 1.0 / 1e9;
extern struct timespec timeStart, timeCurrent;
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
//-----------------------------------------------------------------------------
//Extern Functions
extern void classifyAsteroid(Asteroid *a);
extern void colorPowerUps(Asteroid *a, as_PowerUp *powerUps);
extern void updatePowerUp(Asteroid *a, as_PowerUp *powerUps,
		const int POWERUP_RATE);
extern void assignColors(as_PowerUp *powerUps);

extern void nick_Lab7();
extern void nick_explosion(float, float);
extern int nick_dead(int, int, Asteroid*);
extern void nick_reset(int*, Asteroid*, int*);
extern void nick_drawContinue(int, int, int, int);
extern void nick_score(int*, int*, float*);
extern void nick_drawAsteroid(Flt,
		float, float, float,
		float, float,
		float, float,
		float);
extern void nick_drawBullet(int, int);
extern void nick_drawShip(int, int, int);
extern int nick_updateGodmode();

extern int jtL_Lab7() ;
extern void jt_menu_check_mouse(XEvent *e);
extern void jt_menu_physics( void );
extern void jt_menu_render( void );


//-----------------------------------------------------------------------------

class Image {
	public:
		int width, height;
		unsigned char *data;
		~Image() { delete [] data; }
		Image(const char *fname) {
			if (fname[0] == '\0')
				return;
			//printf("fname **%s**\n", fname);
			char name[40];
			strcpy(name, fname);
			int slen = strlen(name);
			name[slen-4] = '\0';
			//printf("name **%s**\n", name);
			char ppmname[80];
			sprintf(ppmname,"%s.ppm", name);
			//printf("ppmname **%s**\n", ppmname);
			char ts[100];
			//system("convert eball.jpg eball.ppm");
			sprintf(ts, "convert %s %s", fname, ppmname);
			system(ts);
			//sprintf(ts, "%s", name);
			FILE *fpi = fopen(ppmname, "r");
			if (fpi) {
				char line[200];
				fgets(line, 200, fpi);
				fgets(line, 200, fpi);
				//skip comments and blank lines
				while (line[0] == '#' || strlen(line) < 2)
					fgets(line, 200, fpi);
				sscanf(line, "%i %i", &width, &height);
				fgets(line, 200, fpi);
				//get pixel data
				int n = width * height * 3;			
				data = new unsigned char[n];			
				for (int i=0; i<n; i++)
					data[i] = fgetc(fpi);
				fclose(fpi);
			} else {
				printf("ERROR opening image: %s\n",ppmname);
				exit(0);
			}
			unlink(ppmname);
		}
};
Image img[1] = {"space.jpg"};

class Texture {
	public:
		Image *backImage;
		GLuint backTexture;
		float xc[2];
		float yc[2];
};

class Global {
	public:
		int xres, yres;
		char keys[65536];
		Texture tex;
		Global() {
			xres = 700;
			yres = 1000;
			memset(keys, 0, 65536);
		}
} gl;

class Ship {
	public:
		Vec dir;
		Vec pos;
		Vec vel;
		float angle;
		float color[3];
	public:
		Ship() {
			VecZero(dir);
			pos[0] = (Flt)(gl.xres/2);
			pos[1] = (Flt)(30);
			pos[2] = 0.0f;
			VecZero(vel);
			angle = 0.0;
			color[0] = color[1] = color[2] = 1.0;
		}
};



class Game {
	public:
		Ship ship;
		Asteroid *ahead;	
		Bullet *barr;
		as_PowerUp *powerUps;
		int dead;
		int lives;
		int godmode;
		int max_asteroids;
		int nasteroids;
		int nbullets;
		int asterdestroyed;
		float firerate;
		float asteroid_vel_max;
		struct timespec bulletTimer;
		struct timespec mouseThrustTimer;
		bool mouseThrustOn;
	public:
		Game() {
			godmode = 0;
			dead = 0;
			lives = 3;
			max_asteroids = 3;
			firerate = 0.3;
			asteroid_vel_max = 5.0;
			ahead = NULL;
			barr = new Bullet[MAX_BULLETS];
			powerUps = new as_PowerUp[MAX_POWERUPS];
			assignColors(powerUps);
			powerUps[2].stock = 3;
			nasteroids = 0;
			nbullets = 0;
			asterdestroyed = 0;
			mouseThrustOn = false;
			//build 10 asteroids...
			for (int j=0; j<1; j++) {
				Asteroid *a = new Asteroid;
				if (a->sequence < POWERUP_RATE) {
					classifyAsteroid(a);
				}
				a->radius = rnd()*20.0 + 30.0;
				a->pos[0] = (Flt)(rand() % gl.xres);
				a->pos[1] = (Flt)(gl.yres + 100);
				a->pos[2] = 0.0f;
				a->angle = 0.0;
				a->rotate = rnd() * 0.1 - 0.05;
				if (a->type != -1) {
					colorPowerUps(a, powerUps);
				} else {
					a->color[0] = 0.27;
					a->color[1] = 0.15;
					a->color[2] = 0.04;
				}

				a->vel[0] = 0;
				a->vel[1] = 0;
				//std::cout << "asteroid" << std::endl;
				//add to front of linked list
				a->next = ahead;
				if (ahead != NULL)
					ahead->prev = a;
				ahead = a;
				++nasteroids;
			}
			clock_gettime(CLOCK_REALTIME, &bulletTimer);
		}
		~Game() {
			delete [] barr;
		}
} g;


//X Windows variables
class X11_wrapper {
	private:
		Display *dpy;
		Window win;
		GLXContext glc;
	public:
		X11_wrapper() {
			GLint att[] =
			{ GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
			//GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
			XSetWindowAttributes swa;
			setup_screen_res(gl.xres, gl.yres);
			dpy = XOpenDisplay(NULL);
			if (dpy == NULL) {
				std::cout << "\n\tcannot connect to X server"
					<< std::endl;
				exit(EXIT_FAILURE);
			}
			Window root = DefaultRootWindow(dpy);
			XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
			if (vi == NULL) {
				std::cout << "\n\tno appropriate visual found\n"
					<< std::endl;
				exit(EXIT_FAILURE);
			} 
			Colormap cmap =
				XCreateColormap(dpy, root, vi->visual, AllocNone);
			swa.colormap = cmap;
			swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
				PointerMotionMask | MotionNotify | ButtonPress |
				ButtonRelease |
				StructureNotifyMask | SubstructureNotifyMask;
			win = XCreateWindow(dpy, root, 0, 0, gl.xres, gl.yres, 0,
					vi->depth, InputOutput, vi->visual,
					CWColormap | CWEventMask, &swa);
			set_title();
			glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
			glXMakeCurrent(dpy, win, glc);
			show_mouse_cursor(0);
		}
		~X11_wrapper() {
			XDestroyWindow(dpy, win);
			XCloseDisplay(dpy);
		}
		void set_title() {
			//Set the window title bar.
			XMapWindow(dpy, win);
			XStoreName(dpy, win, "Asteroids template");
		}
		void check_resize(XEvent *e) {
			//The ConfigureNotify is sent by the
			//server if the window is resized.
			if (e->type != ConfigureNotify)
				return;
			XConfigureEvent xce = e->xconfigure;
			if (xce.width != gl.xres || xce.height != gl.yres) {
				//Window size did change.
				reshape_window(xce.width, xce.height);
			}
		}
		void reshape_window(int width, int height) {
			//window has been resized.
			setup_screen_res(width, height);
			glViewport(0, 0, (GLint)width, (GLint)height);
			glMatrixMode(GL_PROJECTION); glLoadIdentity();
			glMatrixMode(GL_MODELVIEW); glLoadIdentity();
			glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
			set_title();
		}
		void setup_screen_res(const int w, const int h) {
			gl.xres = w;
			gl.yres = h;
		}
		void swapBuffers() {
			glXSwapBuffers(dpy, win);
		}
		bool getXPending() {
			return XPending(dpy);
		}
		XEvent getXNextEvent() {
			XEvent e;
			XNextEvent(dpy, &e);
			return e;
		}
		void set_mouse_position(int x, int y) {
			XWarpPointer(dpy, None, win, 0, 0, 0, 0, x, y);
		}
		void show_mouse_cursor(const int onoff) {
			if (onoff) {
				//this removes our own blank cursor.
				XUndefineCursor(dpy, win);
				return;
			}
			//vars to make blank cursor
			Pixmap blank;
			XColor dummy;
			char data[1] = {0};
			Cursor cursor;
			//make a blank cursor
			blank = XCreateBitmapFromData (dpy, win, data, 1, 1);
			if (blank == None)
				std::cout << "error: out of memory." << std::endl;
			cursor = XCreatePixmapCursor(dpy, blank, blank, &dummy, &dummy,
					0, 0);
			XFreePixmap(dpy, blank);
			//this makes you the cursor. then set it using this function
			XDefineCursor(dpy, win, cursor);
			//after you do not need the cursor anymore use this function.
			//it will undo the last change done by XDefineCursor
			//(thus do only use ONCE XDefineCursor
			//and then XUndefineCursor):
		}
} x11;

//function prototypes

void init_opengl(void);
void check_mouse(XEvent *e);
void game_check_mouse(XEvent *e);
int check_keys(XEvent *e);
void physics();
void game_physics_dead();
void game_physics();
void render();
void game_render();


//==========================================================================
// M A I N
//==========================================================================
int main()
{
	logOpen();
	init_opengl();
	srand(time(NULL));
	x11.set_mouse_position(100, 100);
	int done=0;
	while (!done) {
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			check_mouse(&e);
			done = check_keys(&e);
		}
		physics();
		render();
		x11.swapBuffers();
	}
	cleanup_fonts();
	logClose();
	return 0;
}

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, gl.xres, gl.yres);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//This sets 2D mode (no perspective)
	glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
	//
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);
	//
	//Clear the screen to black
	glClearColor(0.0, 0.0, 0.0, 1.0);
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
	gl.tex.backImage = &img[0];
	glGenTextures(1, &gl.tex.backTexture);
	int w = gl.tex.backImage->width;
	int h = gl.tex.backImage->height;
	glBindTexture(GL_TEXTURE_2D, gl.tex.backTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
			GL_RGB, GL_UNSIGNED_BYTE, gl.tex.backImage->data);
	gl.tex.xc[0] = 0.0;
	gl.tex.xc[1] = 1.;
	gl.tex.yc[0] = 0.0;
	gl.tex.yc[1] = 1.0;

	initialize_fonts();
}

void normalize2d(Vec v)
{
	Flt len = v[0]*v[0] + v[1]*v[1];
	if (len == 0.0f) {
		v[0] = 1.0;
		v[1] = 0.0;
		return;
	}
	len = 1.0f / sqrt(len);
	v[0] *= len;
	v[1] *= len;
}

void check_mouse(XEvent *e) {
	switch (program_state){
		case GAME:
			game_check_mouse(e);
			break;

		case MENU:
			jt_menu_check_mouse(e);
			break;
	}
}




void game_check_mouse(XEvent *e)
{
	//Was a mouse button clicked?
	static int savex = 0;
	static int savey = 0;
	static int ct=0;
	const float BULLET_COLOR = 1.0f;
	if (e->type != ButtonPress &&
			e->type != ButtonRelease &&
			e->type != MotionNotify)
		return;
	if (e->type == ButtonRelease)
		return;
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button is down
			}
		}
		if (e->xbutton.button==3) {
			//Right button is down
		}
	}
	if (e->type == MotionNotify) {
		if (savex != e->xbutton.x || savey != e->xbutton.y) {
			//Mouse moved
			int xdiff = savex - e->xbutton.x;
			if (++ct < 10)
				return;		
			//mouse moved along the x-axis.
			x11.set_mouse_position(100, 100);
			savex = savey = 100;
		}
	}
}

int check_keys(XEvent *e)
{
	//keyboard input?
	static int shift=0;
	if (e->type != KeyPress && e->type != KeyRelease)
		return 0;
	int key = (XLookupKeysym(&e->xkey, 0) & 0x0000ffff);
	//Log("key: %i\n", key);
	if (e->type == KeyRelease) {
		gl.keys[key]=0;
		if (key == XK_Shift_L || key == XK_Shift_R)
			shift=0;
		return 0;
	}
	gl.keys[key]=1;
	if (key == XK_Shift_L || key == XK_Shift_R) {
		shift=1;
		return 0;
	}
	(void)shift;
	switch (key) {
		case XK_Escape:
			return 1;
		case XK_f:
			break;
		case XK_s:
			break;
		case XK_Down:
			break;
		case XK_equal:
			break;
		case XK_minus:
			break;
	}
	return 0;
}

void deleteAsteroid(Game *g, Asteroid *node)
{
	//Remove a node from doubly-linked list.
	//Must look at 4 special cases below.
	if (node->prev == NULL) {
		if (node->next == NULL) {
			//only 1 item in list.
			g->ahead = NULL;
		} else {
			//at beginning of list.
			node->next->prev = NULL;
			g->ahead = node->next;
		}
	} else {
		if (node->next == NULL) {
			//at end of list.
			node->prev->next = NULL;
		} else {
			//in middle of list.
			node->prev->next = node->next;
			node->next->prev = node->prev;
		}
	}
	delete node;
	node = NULL;
}

void buildAsteroidFragment(Asteroid *ta, Asteroid *a)
{

	extern void physicSwitch(Asteroid *a, bool value);
	//build ta from a
	physicSwitch(ta, true);
	ta->radius = a->radius / 2.0;
	ta->pos[0] = a->pos[0] + rnd()*10.0-5.0;
	ta->pos[1] = a->pos[1] + rnd()*10.0-5.0;
	ta->pos[2] = 0.0f;
	ta->angle = 0.0;
	ta->rotate = a->rotate + (rnd() * 0.1 - 0.05);
	ta->color[0] = a->color[0];
	ta->color[1] = a->color[1];
	ta->color[2] = a->color[2];
	ta->vel[0] = (rnd()*10.0-5);
	ta->vel[1] = (rnd()*10.0-5);
	ta->sequence = a->sequence;
}

void physics()
{
	switch (program_state) {
		case GAME:
			if (!g.dead)
				game_physics();
			if (g.dead)
				game_physics_dead();
		case MENU:
			jt_menu_physics();
	}
}

void game_physics_dead()
{
	if (gl.keys[XK_y] && g.lives) {
		nick_reset(&g.dead, g.ahead, &g.nasteroids);
		g.lives--;
	}
	if (gl.keys[XK_n]) {
		//if we get a menu, make this go to menu instead
		exit(0);
	}
	return;
}
void game_physics()
{
	if (g.godmode) 
		g.godmode = nick_updateGodmode();
	if (g.nasteroids < g.max_asteroids){
		Asteroid *a = new Asteroid;
		if (a->sequence < POWERUP_RATE) {
			classifyAsteroid(a);
		}
		a->radius = rnd()*20.0 + 30.0;
		a->pos[0] = (Flt)(rand() % gl.xres);
		a->pos[1] = (Flt)(rand() % gl.yres + gl.yres + 100);
		a->pos[2] = 0.0f;
		a->angle = 0.0;
		a->rotate = rnd() * 0.1 - 0.05;
		if (a->type != -1) {
			colorPowerUps(a, g.powerUps);
		} else {
			a->color[0] = 0.27;
			a->color[1] = 0.15;
			a->color[2] = 0.04;
		}
		a->vel[0] = 0;
		a->vel[1] = 0;
		//std::cout << "asteroid" << std::endl;
		//add to front of linked list
		a->next = g.ahead;
		if (g.ahead != NULL) {
			g.ahead->prev = a;
		}
		g.ahead = a;
		++g.nasteroids;
	}


	Flt d0,d1,dist;
	//Update ship position
	g.ship.pos[0] += g.ship.vel[0];
	g.ship.pos[1] += g.ship.vel[1];
	//Check for collision with window edges
	if (g.ship.pos[0] < 0.0) {
		g.ship.pos[0] = 0.0;
	}
	else if (g.ship.pos[0] > (float)gl.xres) {
		g.ship.pos[0] = (float)gl.xres;
	}
	//Update bullet positions
	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);
	int i=0;
	while (i < g.nbullets) {
		Bullet *b = &g.barr[i];
		//move the bullet
		b->pos[0] += b->vel[0];
		b->pos[1] += b->vel[1];
		//Check for collision with window edges
		if (b->pos[0] < 0.0 || 
				b->pos[0] > (float)gl.xres || 
				b->pos[1] < 0.0 ||
				b->pos[1] > (float)gl.yres) {
			memcpy(&g.barr[i], &g.barr[g.nbullets-1],
					sizeof(Bullet));
			g.nbullets--;
			continue;
		}
		i++;
	}
	//
	//Update asteroid positions
	Asteroid *a = g.ahead;
	//checks if asteroid has physics turn on or off
	while (a) {
		a->vel[1] += gravity;
		if (abs(a->vel[1]) > g.asteroid_vel_max)
			a->vel[1] = -g.asteroid_vel_max;
		if (abs(a->vel[0]) > g.asteroid_vel_max) {
			if (a->vel[0] < 0) {
				a->vel[0] = -(g.asteroid_vel_max);
			} else {
				a->vel[0] = g.asteroid_vel_max;
			}
		}
		a->pos[0] += a->vel[0];
		a->pos[1] += a->vel[1];
		//Check for collision with window edges
		if (a->pos[0] < 0.0 || a->pos[0] > (float)gl.xres) {
			a->vel[0] = -(a->vel[0]);
		}
		else if (a->pos[1] < -100.0) {
			a->pos[1] += (float)gl.yres+200;
			Asteroid *savea = a->next;
			deleteAsteroid(&g, a);
			if(g.ahead == NULL) {
				a = savea;
			} else { 
				a = g.ahead;

			}			
			g.nasteroids--;
		}
		a->angle += a->rotate;
		a = a->next;
	}

	//
	//Asteroid collision with bullets?
	//If collision detected:
	//   1. delete the bullet
	//   2. break the asteroid into pieces
	//      if asteroid small, delete it
	a = g.ahead;
	while (a) {
		if (nick_dead(g.ship.pos[0], g.ship.pos[1], a) && !g.godmode) {
			g.dead = 1;
		}
		//is there a bullet within its radius?
		int i=0;
		while (i < g.nbullets) {
			Bullet *b = &g.barr[i];
			d0 = b->pos[0] - a->pos[0];
			d1 = b->pos[1] - a->pos[1];
			dist = (d0*d0 + d1*d1);
			if (dist < (a->radius*a->radius) && !b->super) {
				//cout << "asteroid hit." << endl;
				//this asteroid is hit.
				updatePowerUp(a, g.powerUps, POWERUP_RATE);
				if (a->radius >= MINIMUM_ASTEROID_SIZE) {
					//break it into pieces.
					Asteroid *ta = a;
					buildAsteroidFragment(ta, a);
					int r = rand()%3+3;
					for (int k=0; k<r; k++) {
						//get the next asteroid position
						//in the array
						Asteroid *ta = new Asteroid;
						buildAsteroidFragment(ta, a);
						//add to front of asteroid
						//linked list
						ta->next = g.ahead;
						if (g.ahead != NULL) {
							g.ahead->prev = ta;
						}
						g.ahead = ta;
						g.nasteroids++;
					}
					nick_score(&g.asterdestroyed, &g.max_asteroids,
							&g.asteroid_vel_max);
				} else {
					a->color[0] = 1.0;
					a->color[1] = 0.1;
					a->color[2] = 0.1;
					//asteroid is too small to break up
					//delete the asteroid and bullet
					Asteroid *savea = a->next;
					deleteAsteroid(&g, a);
					nick_score(&g.asterdestroyed, &g.max_asteroids,
							&g.asteroid_vel_max);
					a = savea;
					g.nasteroids--;
				}
				//delete the bullet...
				if(!b->super) {
					memcpy(&g.barr[i], &g.barr[g.nbullets-1],
							sizeof(Bullet));
					g.nbullets--;
				}
				if (a == NULL) {
					break;
				}
			}
			if (((dist - SUPER < (a->radius*a->radius)) ||
						(dist + SUPER < (a->radius*a->radius)) ||
						(dist < (a->radius*a->radius))) && b->super) {
				//this asteroid is hit.
				if (0) {
					//break it into pieces.
					Asteroid *ta = a;
					buildAsteroidFragment(ta, a);
					int r = rand()%3+3;
					for (int k=0; k<r; k++) {
						//get the next asteroid position
						//in the array
						Asteroid *ta = new Asteroid;
						buildAsteroidFragment(ta, a);
						//add to front of asteroid
						//linked list
						ta->next = g.ahead;
						if (g.ahead != NULL){
							g.ahead->prev = ta;
						}
						g.ahead = ta;
						g.nasteroids++;
						nick_score(&g.asterdestroyed, &g.max_asteroids,
								&g.asteroid_vel_max);
					}
				} else {
					a->color[0] = 1.0;
					a->color[1] = 0.1;
					a->color[2] = 0.1;
					//asteroid is too small to break up
					//delete the asteroid and bullet
					Asteroid *savea = a->next;
					deleteAsteroid(&g, a);
					nick_score(&g.asterdestroyed, &g.max_asteroids,
							&g.asteroid_vel_max);
					a = savea;
					g.nasteroids--;
				}
				//delete the bullet...
				if(!b->super) {
					memcpy(&g.barr[i], &g.barr[g.nbullets-1],
							sizeof(Bullet));
					g.nbullets--;
				}
				if (a == NULL)
					break;
			}
			i++;
		}
		if (a == NULL){
			break;
		}
		a = a->next;
	}
	//---------------------------------------------------
	//check keys pressed now
	if (gl.keys[XK_Left]) {
		g.ship.pos[0] -= SHIPSPEED;
	}
	if (gl.keys[XK_Right]) {
		g.ship.pos[0] += SHIPSPEED;
	}
	//check ship window edge
	if (g.ship.pos[0] < 0.0) {
		g.ship.pos[0] = 0.0;
	}
	else if (g.ship.pos[0] > (float)gl.xres) {
		g.ship.pos[0] = (float)gl.xres;
	}
	else if (g.ship.pos[1] < 0.0) {
		g.ship.pos[1] += (float)gl.yres;
	}
	else if (g.ship.pos[1] > (float)gl.yres) {
		g.ship.pos[1] -= (float)gl.yres;
	}
	if (gl.keys[XK_space]) {
		//a little time between each bullet
		struct timespec bt;
		clock_gettime(CLOCK_REALTIME, &bt);
		double ts = timeDiff(&g.bulletTimer, &bt);
		if (ts > g.firerate) {
			timeCopy(&g.bulletTimer, &bt);
			if (g.nbullets < MAX_BULLETS) {
				//shoot a bullet...
				//Bullet *b = new Bullet;
				Bullet *b = &g.barr[g.nbullets];
				timeCopy(&b->time, &bt);
				b->pos[0] = g.ship.pos[0];
				b->pos[1] = g.ship.pos[1];
				b->vel[0] = g.ship.vel[0];
				b->vel[1] = g.ship.vel[1];
				//convert ship angle to radians
				Flt rad = ((g.ship.angle+90.0) / 360.0f) *
					PI * 2.0;
				//convert angle to a vector
				Flt xdir = cos(rad);
				Flt ydir = sin(rad);
				b->pos[0] += xdir*20.0f;
				b->pos[1] += ydir*20.0f;
				b->vel[0] += xdir*6.0f + rnd()*0.1;
				b->vel[1] += BULLETSPEED;
				b->color[0] = 1.0f;
				b->color[1] = 1.0f;
				b->color[2] = 1.0f;
				b->super = false;
				g.nbullets++;
			}
		}
	}
	if (gl.keys[XK_b]) {
		if (g.powerUps[3].stock) {
			a = g.ahead;
			while (a) {
				Asteroid *savea = a->next;
				deleteAsteroid(&g, a);
				nick_score(&g.asterdestroyed, &g.max_asteroids,
						&g.asteroid_vel_max);
				a = savea;
				g.nasteroids--;
				g.powerUps[3].stock--;

			} 
		}  
	}

	if (gl.keys[XK_n]) {

		if (g.powerUps[1].stock) {
			g.godmode = 1;
			g.powerUps[1].stock = 0;
		}

	}
	if (gl.keys[XK_v]) {
		if(g.powerUps[0].stock > 0) {
			//a little time between each bullet
			struct timespec bt;
			clock_gettime(CLOCK_REALTIME, &bt);
			double ts = timeDiff(&g.bulletTimer, &bt);
			if (ts > 0.1) {
				timeCopy(&g.bulletTimer, &bt);
				if (g.nbullets < MAX_BULLETS) {
					//shoot a bullet...
					//Bullet *b = new Bullet;
					Bullet *b = &g.barr[g.nbullets];
					timeCopy(&b->time, &bt);
					b->pos[0] = g.ship.pos[0];
					b->pos[1] = g.ship.pos[1] + SUPERSIZE;
					b->vel[0] = g.ship.vel[0];
					b->vel[1] = g.ship.vel[1];
					//convert ship angle to radians
					Flt rad = ((g.ship.angle+90.0) / 360.0f) *
						PI * 2.0;
					//convert angle to a vector
					Flt xdir = cos(rad);
					Flt ydir = sin(rad);
					b->pos[0] += xdir*20.0f;
					b->pos[1] += ydir*20.0f;
					b->vel[0] += xdir*6.0f + rnd()*0.1;
					b->vel[1] += ydir*6.0f + rnd()*0.1;
					b->color[0] = 1.0f;
					b->color[1] = 1.0f;
					b->color[2] = 1.0f;
					b->super = true;
					g.nbullets++;
					g.powerUps[0].stock--;
				}	
			}

		}
	}
	if (g.mouseThrustOn) {
		//should thrust be turned off
		struct timespec mtt;
		clock_gettime(CLOCK_REALTIME, &mtt);
		double tdif = timeDiff(&mtt, &g.mouseThrustTimer);
		if (tdif < -0.3)
			g.mouseThrustOn = false;
	}
	gl.tex.yc[0] -= 0.001;
	gl.tex.yc[1] -= 0.001;
}


void render()
{
	switch (program_state) {
		case GAME:
			game_render();
			break;
		case MENU:
			jt_menu_render();
			break;
	}	
}


void game_render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, gl.tex.backTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(gl.tex.xc[0], gl.tex.yc[1]); glVertex2i(0, 0);
	glTexCoord2f(gl.tex.xc[0], gl.tex.yc[0]); glVertex2i(0, gl.yres);
	glTexCoord2f(gl.tex.xc[1], gl.tex.yc[0]); glVertex2i(gl.xres, gl.yres);
	glTexCoord2f(gl.tex.xc[1], gl.tex.yc[1]); glVertex2i(gl.xres, 0);
	glEnd();

	Rect r;
	//
	r.bot = gl.yres - 20;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, 0x00ffff00, "SCORE: %i",
			g.asterdestroyed);
	ggprint8b(&r, 16, 0x00ffff00, "SuperBullets: %i", g.powerUps[0].stock);

	if (g.powerUps[1].stock) {
		ggprint8b(&r, 16, 0x00ffff00, "GodMode available");
	}
	if (g.powerUps[2].stock) {
		ggprint8b(&r, 16, 0x00ffff00, "Lives: %i", g.lives);
	}
	if (g.powerUps[3].stock) {
		ggprint8b(&r, 16, 0x00ffff00, "Clears available: %i", 
				g.powerUps[3].stock);
	}

	//
	//-------------
	//Draw the ship
	if (!g.dead) {

		nick_drawShip(g.ship.pos[0], g.ship.pos[1], g.godmode);

	}
	//------------------
	//Draw the asteroids
	{
		Asteroid *a = g.ahead;
		while (a) {
			nick_drawAsteroid(a->radius,
					a->color[0], a->color[1], a->color[2],
					a->pos[0], a->pos[1],
					a->vel[0], a->vel[1],
					a->angle);
			a = a->next;
		}
	}
	//----------------
	//Draw the bullets
	Bullet *b = &g.barr[0];
	for (int i=0; i<g.nbullets; i++) {
		//Log("draw bullet...\n");
		if (!b->super){
			nick_drawBullet(b->pos[0], b->pos[1]);
			++b;
		} else {
			float x, y;
			float bright1 = rand()%70;
			float bright2 = rand()%70;
			float r = (255-(bright1))/255;
			float g = (165-(bright2))/255;

			int flicker = rand()%20 - 10;
			glColor3f(r, g, 0.0);
			glBegin(GL_TRIANGLE_FAN);
			for (int i=0; i<180; i++){
				int radius = SUPERSIZE + flicker;

				x = radius * cos(i) + b->pos[0];
				y = radius * sin(i) + b->pos[1];
				glVertex3f (x, y, 0);

				x = radius * cos (i+0.1) + b->pos[0];
				y = radius * sin (i+0.1) + b->pos[1];
				glVertex3f (x, y, 0);
			}
			glEnd();
			bright1 = rand()%70;
			bright2 = rand()%70;
			r = (255-(bright1))/255;
			g = (165-(bright2))/255;

			flicker = rand()%20 - 10;
			glColor3f(r, g, 0.0);
			glBegin(GL_TRIANGLE_FAN);
			for (int i=0; i<180; i++){
				int radius = SUPERSIZE + flicker - 20;

				x = radius * cos(i) + b->pos[0];
				y = radius * sin(i) + b->pos[1];
				glVertex3f (x, y, 0);

				x = radius * cos (i+0.1) + b->pos[0];
				y = radius * sin (i+0.1) + b->pos[1];
				glVertex3f (x, y, 0);
			}
			glEnd();
			++b;
		}
	}

	if (g.dead)
	{
		nick_explosion(g.ship.pos[0], g.ship.pos[1]);
		nick_drawContinue(gl.xres, gl.yres, g.lives, g.asterdestroyed);
	}	

}






