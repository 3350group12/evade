#ifndef as_PowerUp_h
#define as_PowerUp_h

enum powerUp_t {
	SuperBullet = 1,
	GodMode,
	ExtraLife,
	Clear,
	Liberate,
	END = Clear,
};

class as_PowerUp {
public: 
	powerUp_t powerUp;
	char* powerUP;
	int amount;
	as_PowerUp();
	int color[3];


};

#endif
