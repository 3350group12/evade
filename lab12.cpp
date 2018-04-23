
//CMPS 3350
//lab12.cpp
//
//
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <fstream>
using namespace std;

//frequency definitions
#define A4 440.0
#define F0 A4
#define F0_note A
#define F0_octave 4

enum note_t {
	C = 1,
	Cs,
	D,
	Ds,
	E,
	F,
	Fs,
	G,
	Gs,
	A,
	As,
	B,
	END = B,
	HALF_STEPS_PER_OCTAVE = B 
};

double freq(note_t note, int octave_delta);
void unit_test(note_t note, int octave_delta, double tolerance);


int main(int argc, char *argv[])
{
	note_t note;
	int octave_delta;
    double tolerance;

	if (argc != 4) {
		cout << "Usage: " << argv[0] << " <NOTE>  <OCTAVE_DELTA>  <TOLERANCE>" << endl;
		return 0;
	}
	//
	note = (note_t)(toupper(argv[1][0]) - 64);
	octave_delta = atoi(argv[2]);
    tolerance = atof(argv[3]);
	switch(toupper(argv[1][0])) {
		case 'A': note = A; break;
		case 'B': note = B; break;
		case 'C': note = C; break;
		case 'D': note = D; break;
		case 'E': note = E; break;
		case 'F': note = F; break;
	}
	//You may call your unit test here...
    unit_test(note, octave_delta, tolerance);

    if (note > END && octave_delta == 0) {
        return 1;
    }


	if (note > END) {
		cout << "Invalid note!" << endl;
		return 1;
	}

	cout << HALF_STEPS_PER_OCTAVE << endl;
	cout << freq(note, octave_delta) << endl;
	return 0;
}

//-----------------------------------------------------------
//Generate a frequency in hz that is half_steps away from C_4
//Do not modify this function.
//-----------------------------------------------------------
double freq(note_t note, int octave_delta)
{
	double freq;
	double factor;
	double a;
	int n;
	int octave = octave_delta - F0_octave;

	a = pow(2.0, 1.0/(double)HALF_STEPS_PER_OCTAVE);
	n = note - F0_note;
	freq = F0 * pow(a, (double)n);
	factor = pow(2.0, (double)octave);
	freq = freq * factor;
	return freq;
}

void unit_test(note_t note, int octave_delta, double tolerance)
{
    if (note <= END || octave_delta > 0) {
        return;
    }

    ifstream fin;
    double frequency[108];
    double estimate[108];
    double difference[108];
    int bad = 0;
    
    fin.open("data.txt", ios::in);
    
    int i = 0;
    while (fin >> frequency[i]) {
        i++;
    }

    fin.close();

    int tracker = 0;

    for (int i = 0; i < 9; i++) {
        for (int j = 1; j < 13; j++) {
            switch(j) {
                case 1: note = C; break;
                case 2: note = Cs; break;
                case 3: note = D; break;
                case 4: note = Ds; break;
                case 5: note = E; break;
                case 6: note = F; break;
                case 7: note = Fs; break;
                case 8: note = G; break;
                case 9: note = Gs; break;
                case 10: note = A; break;
                case 11: note = As; break;
                case 12: note = B; break;
            }
            estimate[tracker] = freq(note, i);
            tracker++;
        }
    }

    for (int i = 0; i < 108; i++) {
        double temp;
        temp = frequency[i] - estimate[i];
        difference[i] = fabs(temp);
    }

    cout << "Tolerance: " << tolerance << endl;
    cout << "Frequency Function Unit-test" << endl << endl;
                
    cout << left << setw(5) << "note" << setw(7) << "octave" << setw(9)
        << "value" << setw(13) << "diff" << endl;

    cout << left << setw(5) << "----" << setw(7) << "------" << setw(9)
        << "-------" << setw(14) << "----------" << endl;

    for (int i = 0; i < 108; i++) {
        int note2 = i % 12 + 1;
        int octave = i / 12;
        cout << left << " " << setw(4) << note2 << "  " << setw(5) << octave
            << setw(9) << setprecision(6) << estimate[i] << setw(12)
            << setprecision(6) << difference[i];
        if (difference[i] < tolerance) {
            cout << left << setw(12) << "   good" << endl;
        } else {
            bad++;
            cout << left << setw(12) << "<------ bad" << endl;
        }
    }

    cout << "Bad element count: " << bad << endl;
    cout << "Unit test complete." << endl;
    return;


}

