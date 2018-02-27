#ifndef Asteroid_h
#define Asteroid_h

#include <cstdlib>

//defined types 
typedef float Flt; 
typedef float Vec[3]; 
typedef Flt     Matrix[4][4]; 
 
//macros 
#define rnd() (((Flt)rand())/(Flt)RAND_MAX) 
#define random(a) (rand()%(a)) 
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0 
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z) 
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2] 
#define VecDot(a,b)     ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2]) 
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \ 											(c)[1]=(a)[1]-(b)[1]; \ 							(c)[2]=(a)[2]-(b)[2]


class Asteroid {
public:
        Vec pos;
        Vec vel;
        int nverts;
        int physics;
        Flt radius;
        Vec vert[8];
        float angle;
        float rotate;
        float color[3];
        struct Asteroid *prev;
        struct Asteroid *next;
public:
        Asteroid() {
                //Physics for asteroids won't work, hence it asteroids will stay in place
                physics = false;
                prev = NULL;
                next = NULL;
        }
};

#endif 
