#ifndef Bullet_h
#define Bullet_h

#include <cstdlib>
class Bullet {
public:
        Vec pos;
        Vec vel;
        bool super;
        float color[3]; 
        struct timespec time;
public:
        Bullet() { }
};

#endif 
