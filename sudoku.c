#include <ncurses.h>

/* Arbitrary scaling factors used to ensure that display is vaguely square */
#define W_STREATCH 4
#define H_STREATCH 2

/* Width and height of main window */
#define W_WIDTH (8 * W_STREATCH + 3) 
#define W_HEIGHT (8 * H_STREATCH + 3)

/* Get coords on window given coords for grid */
#define wx(G_X) (G_X * W_STREATCH + 1) 
#define wy(G_Y) (G_Y * H_STREATCH + 1)


WINDOW * win; //Main window
int grid[9][9]; //Grid that holds numerical values
int initgrid[9][9]; //Initial grid, used for saving initial state

/* Cursor coordinates (in terms of grid) */
int c_y;
int c_x;

int drawwin(); //Draw current window state
int main_loop(); //Main loop of program

int main () {
	c_x = c_y = 0;
	initscr();
	cbreak();
	noecho();
	erase();
//	printw("Why isn't this working?\n");

	win = newwin(W_HEIGHT, W_WIDTH, 2, 2);
	keypad(win, TRUE);
	int x, y;
	for (x = 0; x < 9; x++)
		for (y = 0; y < 9; y++) {
			initgrid[x][y] = 0
			grid[x][y] = 0; //Temporary, until grids can be created
		}

	drawwin();
//	wmove(win, wy(0), wx(0));
//	refresh();
	main_loop();






//	while (wgetch(win) != 'x') {
//		printw("Why isn't this working?\n");
//		refresh();
//	}
//	getch();
	endwin();

	return 0;
}

int drawwin() {
	werase(win);
	box(win, 0, 0);
	int ix, iy;
	for (iy = 0; iy < 9; iy++)
		for (ix = 0; ix < 9; ix++) {
			if (grid[ix][iy]) mvwaddch(win, wy(iy), wx(ix), '0' + grid[ix][iy]);
			else mvwaddch(win, wy(iy), wx(ix), ' ');
		}
	for (ix = 1; ix < 3; ix++) {

		wmove(win, wy(ix * 3) - H_STREATCH / 2, 1);
		whline(win, ACS_HLINE, W_WIDTH - 2);
		mvwaddch(win, wy(ix * 3) - H_STREATCH / 2, 0, ACS_LTEE);
		mvwaddch(win, wy(ix * 3) - H_STREATCH / 2, W_WIDTH - 1, ACS_RTEE);

		wmove(win, 1, wx(ix * 3) - W_STREATCH / 2);
		wvline(win, ACS_VLINE, W_HEIGHT - 2);
		mvwaddch(win, 0, wx(ix * 3) - W_STREATCH / 2, ACS_TTEE);
		mvwaddch(win, W_HEIGHT - 1, wx(ix * 3) - W_STREATCH / 2, ACS_BTEE);

		mvwaddch(win, wy(ix * 3) - H_STREATCH / 2, wx(ix * 3) - W_STREATCH / 2, ACS_PLUS);
	}

	mvwaddch(win, wy(3) - H_STREATCH / 2, wx(6) - W_STREATCH / 2, ACS_PLUS);
	mvwaddch(win, wy(6) - H_STREATCH / 2, wx(3) - W_STREATCH / 2, ACS_PLUS);
	wmove(win, wy(c_y), wx(c_x));
	wrefresh(win);
	return 0;
}


int main_loop() {
	int ch;
	while ((ch = wgetch(win)) != 'x') {
		switch (ch) {
//			case 'h':
			case KEY_LEFT:
				c_x--;
				break;
//			case 'l':
			case KEY_RIGHT:
				c_x++;
				break;
			case KEY_UP:
				c_y--;
				break;
			case KEY_DOWN:
				c_y++;
				break;
			case ' ':
			case '0':
				if (!initgrid[c_x][c_y]) { //Don't overwrite initial grid, only change its zeros
					waddch(win, ' ');
					grid[c_x][c_y] = 0;
				}
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (!initgrid[c_x][c_y]) {
					waddch(win, ch);
					grid[c_x][c_y] = ch - '0';
				}
				break;
			case ':': //Special commands with colon - not implemented
				break;
			default:
				break;


		}
		c_x = (c_x + 9) % 9;
		c_y = (c_y + 9) % 9;
		wmove(win, wy(c_y), wx(c_x));
	}


	return 0;
}



