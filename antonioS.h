#ifndef antonioS_h
#define antonioS_h

enum powerUp_t {
	SuperBullet = 1,
	GodMode,
	ExtraLife,
	Clear,
	//Liberate,
	END = Clear,
};

class as_PowerUp {
public: 
	powerUp_t powerUp;
	char* powerUP;
	int stock;
	as_PowerUp();
	void generatePowerUp(int type);
	int color[3];


};


class Neuron {
    public:
        double *feat;
        int numFeat;
        double delta;
        double *weight;
        double theta;
        double charge;
        double output;
        void init(int size);
        void fCharge();
        void activate();
        Neuron();
};


#endif
