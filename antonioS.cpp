#include "Asteroid.h"
#include "Bullet.h"
#include "fonts.h"
#include "antonioS.h"
#include <GL/glx.h>
#include <iostream>
#include <cmath>
#include <ctime>
//defined types 
#define PI 3.141592653589793

using namespace std;

extern struct timespec timeStart, timeCurrent;
extern double timeDiff(struct timespec *start, struct timespec *end);

//turn physics for asteroids on and off
void showName(Rect r, int y_res)
{
	ggprint8b(&r, 16, 0x00ff0000, "Antonio Solorio");

}

void johnDoe(Rect r, int y_res) 
{
    struct timespec start;
    struct timespec end;
    static double seconds = 0, seconds2 = 0;
    long long int love = 1;
    ggprint8b(&r, 16, 0x00ff0000, "Antonio Solorio");
    clock_gettime(CLOCK_REALTIME, &start);
    love = 0;
    for (int i = 0; i < 10000; i++) {
		love = love * 2;
		love = love / 2;
    }
    clock_gettime(CLOCK_REALTIME, &end);
    seconds += timeDiff(&start, &end);
    ggprint8b(&r, 16, 0x00ff0000, "function multiplication process time: %lf"
		, seconds);
    clock_gettime(CLOCK_REALTIME, &start);
    for (int i =0; i < 10000; i++) {
		love = love << 1;
		love = love >> 1;
    }
    clock_gettime(CLOCK_REALTIME, &end);
    seconds2 += timeDiff(&start, &end);
    ggprint8b(&r, 16, 0x00ff0000, "function bit shift optimized time: %lf"
		, seconds2);
}

void drawBox(int x, int y, Rect z)
{
	time_t start, end;
	static float seconds = 0;
	time(&start);
	static float angle = 0.0;
	glColor3ub(255,0,0);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glTranslatef(-50.0, -50.0, 0);
	angle = angle + 2.5;
	glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(0, 100);
		glVertex2i(100, 100);
		glVertex2i(100, 0);
	glEnd();
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
		glVertex2f(0.0f, 0.0f);
	glEnd();
	Rect r;
	r.bot = 50;
	r.left = 50;
	r.center = 1;
	ggprint8b(&r, 16, 0x00ffffff, "Antonio Solorio");
	glPopMatrix();
	time(&end);
	seconds += difftime(end, start);
//	ggprint8b(&z, 16, 0x00ff0000, "drawBox time: %lf", seconds);


}

void physicSwitch(Asteroid *a, bool value) 
{
	if (value) {
		a->physics = true;
	} else {
		a->physics = false;
	}
}

bool checkPhysics(Asteroid *a) 
{
	if (a->physics) {
		return true;
	} else {
		return false;
	}
}


void superifyBullet(Bullet *b)
{
	b->super = true;
	return;
}

as_PowerUp::as_PowerUp()
{
	stock = 0;

}

void as_PowerUp::generatePowerUp()
{
	int temp = rand() % 100;
	if (temp < 90) {
		powerUp = SuperBullet;
	    color[0] = 255;
		color[1] = 165;
		color[2] = 0;
		stock = 1;
	} 
    else if (temp >= 90 || temp < 96) {
		powerUp = GodMode;
		color[0] = 0;
		color[1] = 191;
		color[2] = 255;
        stock = 1;
    }
    else if (temp >= 96 || temp < 98) {
        temp = 2;
		powerUp = ExtraLife;
		color[0] = 127;
		color[1] = 255;
		color[2] = 0;
		stock = 1;
    }
    else {
		powerUp = Clear;
		color[0] = 0;
		color[1] = 0;
		color[2] = 0;
		stock = 1;
    }
    
	printf("PowerUp generated!");

}


void classifyAsteroid(Asteroid *a, as_PowerUp *powerUps) 
{
	//generates powerUP based on Asteroid classification number

	if (a->sequence < 5) {
		as_PowerUp temp;
		temp.generatePowerUp();
		switch(temp.powerUp) {
			case SuperBullet:
				printf("Asteroid classified as SuperBullet Powerup!\n");
				powerUps[0].stock += temp.stock;
				break;
			case GodMode:
				printf("Asteroid classified as GodMode Powerup!\n");
                if (!powerUps[1].stock) {
				    powerUps[1].stock += temp.stock;
                }
				break;
			case ExtraLife:
				printf("Asteroid classified as ExtraLife Powerup!\n");
				powerUps[2].stock += temp.stock;
				break;
			case Clear:
				printf("Asteroid classified as CLearPowerup!\n");
				powerUps[3].stock += temp.stock;
				break;
				
		
		}
	}
}

void destroyShip() 
{



}

void colorPowerUps (Asteroid *a) 
{






}


void decreaseLives (as_PowerUp *powerUps) 
{
    powerUps[2].stock--;    

}
