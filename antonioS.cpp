#include "Asteroid.h"
#include <iostream>
#include <cmath>
//defined types 
#define PI 3.141592653589793


using namespace std;
//turn physics for asteroids on and off
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

