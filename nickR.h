//Nick Rosbrugh Header File


class Star {
public:
    Vec pos;
    int starx[20];
    int stary[20];
    struct Star *prev;
    struct Star *next;
public:
    Star();
};
