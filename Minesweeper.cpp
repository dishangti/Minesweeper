#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <limits>

using namespace std;

struct DIR {
	int dx, dy;
};

const char MINE = 0x80,
					VIS = 0x40,
			  	  MSIGN = 0x20,
	  			  DOUBT = 0x10,
	  			  MINES = 0x0F;  
const char PUTMSIGN = 'p',
			  	  GETSIGN = 'g',
	 			   PUTDOUBT = 'a',
	  			  CLICK = 'c',
	  			  HELP = 'h',
	  			  YES = 'y',
	  			  NO = 'n';
const char OMSIGN = '#',
		 		   ODOUBT = '?',
			 	   ONORMAL = '*';
const int RNDDIRS = 8, NEXTDIRS = 4, OWIDTH = 3;
const DIR rnddir[RNDDIRS] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}},
				  nextdir[NEXTDIRS] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

char **mp;
int w, h, mines, restmines, restblocks, msigns, first, status, quit;
double t1, t2;

inline void addmines(int x, int y)
{
	mp[y][x]++;
}

inline int getmines(int x, int y)
{
	return mp[y][x] & MINES;
}

inline void setsign(int x, int y, char type)
{
	mp[y][x] |= type;
}

inline void clssign(int x, int y)
{
	mp[y][x] &= ~(MSIGN | DOUBT);
}

inline int check(int x, int y, char type)
{
	return mp[y][x] & type;
}

inline void help()
{
	cout <<"c x y: Click (x, y)" << endl
			 << "p x y: Put a sign of mine on (x, y)" << endl
			 << "a x y: Put a sign of doubt on (x, y)" << endl
			 << "g x y: Clear the sign from (x, y)" << endl
			 << "h: Output help information" << endl;
}

inline int checkcin()
{
	if (cin.fail()) {
		cerr << "Input error." << endl;
		cin.clear(istream::goodbit);
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return 1;
	}
	return 0;
}

void print()
{
	cout << endl;
	cout << left << setw(OWIDTH) << ' ';
	for (int i = 1; i <= w; i++) cout << left << setw(OWIDTH) << i;

	cout << endl;
	for (int i = 1; i <= h; i++) {
		cout << left << setw(OWIDTH) << i;
		for (int j = 1; j <= w; j++) {
			if (check(j, i, VIS)) {
				if (getmines(j, i)) cout << left << setw(OWIDTH) << getmines(j, i);
				else cout << left << setw(OWIDTH) << ' ';
			} else {
				if (check(j, i, MSIGN)) cout << left << setw(OWIDTH) << OMSIGN;
				else if (check(j, i, DOUBT)) cout << left << setw(OWIDTH) << ODOUBT;
				else cout << left << setw(OWIDTH) << ONORMAL;
			}
		}
		cout << left << setw(OWIDTH) << i << endl;
	}

	cout << left << setw(OWIDTH) << ' ';
	for (int i = 1; i <= w; i++) cout << left << setw(OWIDTH) << i;
	cout << endl;
}

void setmine(int x, int y)
{
	srand((unsigned int)time(NULL));
	for (int i = 1; i <= mines; i++) {
		int tx, ty;
		tx = (int)floor((double)rand() / RAND_MAX * w) % w + 1;
		ty = (int)floor((double)rand() / RAND_MAX * h) % h + 1;
		if (check(tx, ty, MINE) || (tx == x && ty == y)) {
			i--;
			continue;
		}
		setsign(tx, ty, MINE);
		
		int sx, sy, fx, fy;
		sx = tx - 1; sy = ty - 1;
		fx = tx + 1; fy = ty + 1;
		if (sx < 1) sx = 1; if (sy < 1) sy = 1;
		if (fx > w) fx = w; if (fy > h) fy = h;
		for (int py = sy; py <= fy; py++)
			for (int px = sx; px <= fx; px++) addmines(px, py);
	}
}

int statrnd(int x, int y)
{
	int correct = 0, sum = 0;
	for (int i = 0; i < RNDDIRS; i++) {
		int tx, ty;
		tx = x + rnddir[i].dx;
		ty = y + rnddir[i].dy;
		if (tx < 1 || tx > w) continue;
		if (ty < 1 || ty > h) continue;
		if (check(tx, ty, MSIGN)) {
			if (check(tx, ty, MINE)) correct++;
			sum++;
		}
	}
	if (sum && sum == correct && getmines(x, y) && getmines(x, y) == correct) return 1;
	return 0;
}

void dfsvis(int x, int y)
{
	if (check(x, y, VIS | MINE | MSIGN)) return;
	setsign(x, y, VIS);
	restblocks--;
	
	if (statrnd(x, y)) {
		for (int i = 0; i < NEXTDIRS; i++) {
			int tx, ty;
			tx = x + nextdir[i].dx;
			ty = y + nextdir[i].dy;
			if (tx < 1 || tx > w) continue;
			if (ty < 1 || ty > h) continue;
			dfsvis(tx, ty);
		}
	} else {
		for (int i = 0; i < RNDDIRS; i++) {
			int tx, ty;
			tx = x + rnddir[i].dx;
			ty = y + rnddir[i].dy;
			if (tx < 1 || tx > w) continue;
			if (ty < 1 || ty > h) continue;
			if (getmines(tx, ty) && !check(tx, ty, VIS | MSIGN | MINE)) {
				setsign(tx, ty, VIS);
				restblocks--;
			} else dfsvis(tx, ty);
		}
	}
}

void click(int x, int y)
{
	if (check(x, y, MINE)) {
		status = 2;
		return;
	}
	if (check(x, y, VIS)) {
		cout << "Having been swept." << endl;
		return;
	}
	if (getmines(x, y) && !statrnd(x, y)) {
				setsign(x, y, VIS);
				restblocks--;
	} else dfsvis(x, y);
}

void input()
{
	char cmd;
	int x, y;
	cout << "(MINESWEEPER):";
	cin >> cmd;
	if (cmd == HELP) {
		help();
		return;
	}
	cin >> x >> y;
	if (checkcin()) return;
	if (x < 1 || x > w) return;
	if (y < 1 || y > h) return;
	
	if (!first && cmd == CLICK) {
		first = 1;
		setmine(x, y);
		t1 = (double)clock() / CLOCKS_PER_SEC;
		click(x, y);
		return;
	} else if (!first) {
		cout << "Click a block." << endl;
		return;
	}

	if (cmd == PUTMSIGN) {
		if (check(x, y, VIS)) {
			cout << "Having been swept." << endl;
		} else {
			if (!(check(x, y, MSIGN))) {
				if (check(x, y, MINE)) restmines--;
				msigns++;
			}
			setsign(x, y, MSIGN);
		}
	} else if (cmd == PUTDOUBT) {
		if (check(x, y, VIS)) {
			cout << "Having been swept." << endl;
		} else {
			if (check(x, y, MSIGN)) {
				if (check(x, y, MINE)) restmines++;
				msigns--;
			}
			setsign(x, y, DOUBT);
		}
	} else if (cmd == GETSIGN) {
		if (check(x, y, VIS)) {
			cout << "Having been swept." << endl;
		} else {
			if (check(x, y, MSIGN) || check(x, y, DOUBT)) {
					if (check(x, y, MSIGN)) {
						if (check(x, y, MINE)) restmines++;
						msigns--;
					}
					clssign(x, y);
				} else cout << "No sign." << endl;
		}
	} else if (cmd == CLICK) {
		if (check(x, y, VIS)) cout << "Having been swept." << endl;
		else click(x, y);
	} else {
		cout << "Command error" << endl
				 << "Type h for help." << endl;
	}
	if ((!restmines && msigns == mines) || restblocks == mines) status = 1;
}

int init()
{
	cout << "Board Width(1~99):";
	cin >> w;
	if (checkcin()) return 0;
	
	cout << "Board Height(1~99):";
	cin >> h;
	if (checkcin()) return 0;
	
	cout << "Number of Mines(1~W*H/2):";
	cin >> mines;
	if (checkcin()) return 0;
	
	if (mines < 1 || mines > (w * h >> 1)) return 0;
	if (w >= 100 || h >= 100) return 0;
	restmines = mines;
	restblocks = w * h;

	if (!(mp = new char*[h + 1])) return 0;
	for (int i = 0; i <= h; i++)
		if (!(mp[i] = new char[w + 1])) return 0;
	for (int i = 0; i <= h; i++)
		for (int j = 0; j <= w; j++)
			mp[i][j] = 0;
	return 1;
}

void uninit()
{
	if (mp)
		for (int i = 0; i <= h; i++)
			if (mp[i]) delete[] mp[i];
		delete[] mp;
		mp = NULL;
}

void replay()
{
	int correct = 0;
	while (!correct) {
		char choice;
		cout << "Play again?(y/n):";
		cin >> choice;
		if (choice == YES) {
			quit = 0; correct = 1;
		} else if (choice == NO) {
			quit = 1; correct = 1;
		}
	}
}

void succees()
{
	t2 = (double)clock() / CLOCKS_PER_SEC;
	cout << "In " << floor(t2 - t1 + 0.5) << " sec" << endl;
	cout << "Swept all mines." << endl << endl;
	replay();
}

void fail()
{
	t2 = (double)clock() / CLOCKS_PER_SEC;
	cout << "In " << floor(t2 - t1 + 0.5) << " sec" << endl;
	cout << "Click a mine." << endl << endl;
	replay();
}

void play()
{
	status = 0;
	first = 0;
	msigns = 0;

	if (!init()) {
		cerr << "Failed to init." << endl << endl;
		uninit();
		return;
	}
	while (!status) {
		print();
		input();
	}
	
	if (status == 1) succees();
	else if (status == 2) fail();
	uninit();
}

int main()
{
	while (!quit) play();
	return 0;
}
