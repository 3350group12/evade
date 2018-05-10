#include "Asteroid.h"
#include "Bullet.h"
#include "fonts.h"
#include "antonioS.h"
#include <GL/glx.h>
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
//defined types 
#define PI 3.141592653589793
#define RES 700
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

void classifyAsteroid(Asteroid *a) 
{
    int temp = rand() % 100;
    if (temp < 89) {
        a->type = 0;
    }
    else if (temp < 90 && temp >= 89) {
        a->type = 1;
    }
    else if (temp < 95 && temp >= 90) {
        a->type = 2;
    }
    else {
        a->type = 3;
    }

}

void as_PowerUp::generatePowerUp(int type)
{
	if (type == 0) {
		powerUp = SuperBullet;
		stock = 1;
	} 
    else if (type == 1) {
		powerUp = GodMode;
        stock = 1;
    }
    else if (type == 2) {
		powerUp = ExtraLife;
		stock = 1;
    }
    else {
		powerUp = Clear;
		stock = 1;
    }
    
	printf("PowerUp generated!");

}

void assignColors(as_PowerUp *powerUps)
{
    //SuperBullet colors
    powerUps[0].color[0] = 255;
	powerUps[0].color[1] = 165;
	powerUps[0].color[2] = 0;

    //GodMode colors
	powerUps[1].color[0] = 0;
	powerUps[1].color[1] = 191;
	powerUps[1].color[2] = 255;

    //ExtraLife colors
	powerUps[2].color[0] = 127;
	powerUps[2].color[1] = 255;
	powerUps[2].color[2] = 0;
    
    //Clear colors
	powerUps[3].color[0] = 255;
	powerUps[3].color[1] = 255;
	powerUps[3].color[2] = 255;
}

void updatePowerUp(Asteroid *a, as_PowerUp *powerUps, int &lives, const int POWERUP_RATE) 
{
	//generates powerUP based on Asteroid classification number

	if (a->sequence < POWERUP_RATE && a->type != -1) {
		as_PowerUp temp;
		temp.generatePowerUp(a->type);
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
				lives += temp.stock;
				break;
			case Clear:
				printf("Asteroid classified as CLearPowerup!\n");
				powerUps[3].stock += temp.stock;
				break;
		}
	}
}


void colorPowerUps (Asteroid *a, as_PowerUp *powerUps) 
{
    for (int i = 0; i < 3; i++) {
        a->color[i] = (float)powerUps[a->type].color[i]/255.0;
    }

}


void decreaseLives (as_PowerUp *powerUps) 
{
    powerUps[2].stock--;    

}

void uploadScores(int points, char *user, int size = 32) {
    char username[32] = "";
    char command[200] = "curl --silent www.cs.csubak.edu/~asolorio/3350/evade/evade.php?input=";
    strcpy(username, user);
    strcat(command, username);
    char points2[20];
    snprintf(points2, sizeof(points2), "%d", points);
    strcat(command, ",");
    strcat(command, points2);
    system(command);


}


Neuron::Neuron()
{
    //having a constructor really didn't help
}


void Neuron::init(int size)
{
    numFeat = size;
    double min = -2.4/((double)size);
    double max = -2.4/((double)size);
    feat = new double[size];
    weight = new double[size];
    min = min * (1000000);
    max = max * (1000000);
    for (int i = 0; i < size; i++) {
        feat[i] = 0;
        weight[i] = 0;
    }
    double range = max - min + 1;
    for (int i = 0; i < size; i++) {
        weight[i] = rand() % (int)range + min;
        weight[i] = (float)weight[i]/(1000000.0);
    }
    theta = rand() % (int)range + min;
    theta = (double)theta/(1000000.0);
}

void Neuron::fCharge()
{
    output = 0;
    for (int i = 0; i < numFeat; i++) {
        output = output + weight[i] * feat[i];
    }
    output = output - theta;
}

void Neuron::activate() 
{
    double negate, temp;
    negate = -1.0 * (double)output;
    temp = 1.0 * exp(negate);
    charge = (double)1.0/((double)temp);

}

void neuralNetwork(double &pixel, int res, double curPos, double prevPos = -1)
{
    static double ALPHA = 0.10;
    static int epoch = 0;
    static const int DIMENSION = 1;
    static const int HIDDEN = 10;
    static Neuron hidden[HIDDEN];
    static Neuron neuron[RES];

    if (epoch > 0) {
        for (int i = 0; i < res; i++) {
            if (i == curPos) {
                neuron[i].delta = neuron[i].charge * (1 - neuron[i].charge) * (1 - neuron[i].charge);
            } else {
                neuron[i].delta = neuron[i].charge * (1 - neuron[i].charge) * (0 - neuron[i].charge);
            }
        }
        
        for (int i = 0; i < res; i++) {
            for (int j = 0; j < HIDDEN; j++) {
                neuron[i].weight[j] = neuron[i].weight[j] + ALPHA * neuron[i].delta * hidden[j].charge;
            }
            neuron[i].theta = neuron[i].theta + ALPHA * (-1.0) * neuron[i].delta;
        }
        
        for (int i = 0; i < res; i++) {
            int temp = 0;
            for (int j = 0; j < HIDDEN; j++) {
                temp = temp + neuron[i].weight[j] * neuron[i].delta;
            }
            hidden[i].delta = hidden[i].charge * (1 - hidden[i].charge) * temp;
        }

        for (int i = 0; i < HIDDEN; i++) {
            for (int j = 0; j < DIMENSION; j++) {
                hidden[i].weight[j] = hidden[i].weight[j] + ALPHA * hidden[i].feat[j] * hidden[i].delta;
            }
            hidden[i].theta = hidden[i].theta + ALPHA * (-1) * hidden[i].delta;
        }

    }

    for (int i = 0; i < 10; i++) {
        hidden[i].init(DIMENSION);
    }
    
    for (int i = 0; i < res; i++) {
        neuron[i].init(HIDDEN);
    }
    
    for (int i = 0; i < HIDDEN; i++) {
        for (int j = 0; j < DIMENSION; j++) {
            hidden[i].feat[j] = curPos;
        }
    }
    
    for (int i = 0; i < HIDDEN; i++) {
        hidden[i].fCharge();
        hidden[i].activate();
    }
    
    for (int i = 0; i < res; i++) {
        for (int j = 0; j < HIDDEN; j++) {
            neuron[i].feat[j] = hidden[j].charge;
        }
    }

    for (int i = 0; i < res; i++) {
        neuron[i].fCharge();
        neuron[i].activate();
    }

    int tempCharge = -99999;
    for (int i = 0; i < res; i++) {
        if (neuron[i].charge > tempCharge) {
            tempCharge = neuron[i].charge;
            pixel = i;
        }
    }
    epoch++;
}

double calcAngle(double astx, double asty, double predx, double predy) 
{   
    //double PI = 3.14159265;
    double opposite =  astx - predx;
    double diffx = astx - predx;
    double diffy = asty - predy;
    double hypotenuse = sqrt((diffx * diffx) - (diffy * diffy));
    double temp = opposite / hypotenuse;
    double temp2 = acos(temp);
    double angle = acos(temp2) * (180.0/PI);
    return angle;


} 
