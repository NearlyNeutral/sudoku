#include <ncurses.h>

/* Arbitrary scaling factors used to ensure that display is vaguely square */
#define W_STREATCH 4
#define H_STREATCH 2

/* Width and height of main window */
#define W_WIDTH (8 * W_STREATCH + 3) 
#define W_HEIGHT (8 * H_STREATCH + 3)

/* Width and heigh of info window */
#define I_WIDTH 40
#define I_HEIGHT 2

/* Get coords on window given coords for grid */
#define wx(G_X) (G_X * W_STREATCH + 1) 
#define wy(G_Y) (G_Y * H_STREATCH + 1)


WINDOW * win; //Main window
WINDOW * infowin; //Secondary window with commands etc
int grid[9][9]; //Grid that holds numerical values
int initgrid[9][9]; //Initial grid, used for saving initial state

/* Cursor coordinates (in terms of grid) */
int c_y;
int c_x;
int highlighted;

int drawwin(); //Draw current window state
int main_loop(); //Main loop of program
int cal_progress(); //Calculate how many rows/columns/boxes are complete out of 27
int draw_infowin();
int update_complete();
int highlight(int hnum); //Highlight number
int special(); //Do special commands
int goodbye();

int main () {
	c_x = c_y = 0;
	highlighted = 0;
	initscr();
	cbreak();
	noecho();
//	start_color();
//	init_pair(1, COLOR_WHITE, COLOR_BLACK);
//	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	erase();
//	printw("Why isn't this working?\n");

	win = newwin(W_HEIGHT, W_WIDTH, 1, 1);
	infowin = newwin(I_HEIGHT, I_WIDTH, 1 + W_HEIGHT, 2);
	keypad(win, TRUE);
	int x, y;
	for (x = 0; x < 9; x++)
		for (y = 0; y < 9; y++) {
			initgrid[x][y] = 0;
			grid[x][y] = 0; //Temporary, until grids can be created
		}
	initgrid[3][5] = grid[3][5] = 7; //Debug test value

	drawwin();
	draw_infowin();
	wmove(win, wy(c_y), wx(c_x));
//	wmove(win, wy(0), wx(0));
//	refresh();
	main_loop();






//	while (wgetch(win) != 'x') {
//		printw("Why isn't this working?\n");
//		refresh();
//	}
//	getch();
	goodbye();
	endwin();

	return 0;
}

int drawwin() {
	werase(win);
	box(win, 0, 0);
	int ix, iy;
/*	wattron(win, A_BOLD); // May have to be changed if used other than at the immediate start
	for (iy = 0; iy < 9; iy++)
		for (ix = 0; ix < 9; ix++) {
			if (grid[ix][iy]) mvwaddch(win, wy(iy), wx(ix), '0' + grid[ix][iy]);
			else mvwaddch(win, wy(iy), wx(ix), ' ');
		}
	wattroff(win, A_BOLD);*/
	highlight(0);
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
					update_complete();
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
					if ((ch - '0') == highlighted) highlight(highlighted); //Highlight new numbers also
					update_complete();
				}
				break;
			case ':': //Special commands with colon - not implemented
				special();
				break;
			default:
				break;


		}
		c_x = (c_x + 9) % 9;
		c_y = (c_y + 9) % 9;
		wmove(win, wy(c_y), wx(c_x));
		if (cal_progress() == 27) {
			wrefresh(win);
			break;
		}
	}


	return 0;
}


int cal_progress() {
	int pr = 0; //counter value
	int nine[10];
	int i, i2, hol;

	for (i = 0; i < 9; i++) {
		//test all nine columns
		for (i2 = 0; i2 < 10; i2++) //Clear array
			nine[i2] = 0;
		for (i2 = 0; i2 < 9; i2++) //Count up frequency of each number
			nine[grid[i][i2]]++;
		hol = 0;
		for (i2 = 1; i2 < 10; i2++) //Count number of places where frequency is 1 (not including 0)
			if (nine[i2] == 1) 
				hol++;
		if (hol == 9) pr++; //If all nine are one, add 1 to pr
		
		//test rows in same loop
		for (i2 = 0; i2 < 10; i2++)
			nine[i2] = 0;
		for (i2 = 0; i2 < 9; i2++)
			nine[grid[i2][i]]++; //Note: Sole difference is order of variables here.
		hol = 0;
		for (i2 = 1; i2 < 10; i2++)
			if (nine[i2] == 1) 
				hol++;
		if (hol == 9) pr++;
	}

	//To do boxes, need two new variables
	int o, o2; 
	for (i = 0; i < 9; i += 3)
		for (o = 0; o < 9; o +=3) //i and o are top-left coords of each box
		{
			for (i2 = 0; i2 < 10; i2++)
				nine[i2] = 0;
			for (i2 = 0; i2 < 3; i2++)
				for (o2 = 0; o2 < 3; o2++)
					nine[grid[i + i2][o + o2]]++;
			hol = 0;
			for (i2 = 1; i2 < 10; i2++)
				if (nine[i2] == 1) 
					hol++;
			if (hol == 9) pr++;
		}

	return pr;
}

int draw_infowin() {
	werase(infowin);
	mvwprintw(infowin, 0, 0, "Complete:   /27  Highlight no.      ");
	wattron(infowin, A_BOLD);
	mvwprintw(infowin, 0, 31, ":#");
	wattroff(infowin, A_BOLD);
	update_complete();
//	wrefresh(infowin);
	return 0;
}

int update_complete() {
	wattron(infowin, A_BOLD);
	mvwprintw(infowin, 0, 10, "%2d", cal_progress());
	wattroff(infowin, A_BOLD);
	wrefresh(infowin);
	return 0;
}

//Basically redraws the screen - highlight(0) could be used to replace part of drawwin()
int highlight(int hnum) {
	int i, i2;
	for (i = 0; i < 9; i++)
		for (i2 = 0; i2 < 9; i2++) {
			wattrset(win, A_NORMAL);
			if (initgrid[i][i2]) wattron(win, A_BOLD);
			if ((grid[i][i2] == hnum) && (hnum)) wattron(win, A_REVERSE);
			if (grid[i][i2]) mvwaddch(win, wy(i2), wx(i), '0' + grid[i][i2]);
			else mvwaddch(win, wy(i2), wx(i), ' ');
//			if (grid[i][i2] == hnum) wattroff(win, COLOR_PAIR(1));			
			
		}

	wattrset(win, A_NORMAL);
	wrefresh(win);
	highlighted = hnum;
	return 0;
}


int special() {
	mvwaddch(infowin, 1, 0, ':');
	wmove(infowin, 1, 1);
	char c = wgetch(infowin);
	switch (c) { //Currently only functionality is to highlight characters
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':		
			highlight(c - '0');
			break;
		case ' ':
			highlight(0);
			break;
		default:
			break;
	}
	mvwaddch(infowin, 1, 0, ' ');
	wrefresh(infowin);	

	return 0;
}

int goodbye() {
	WINDOW * gbye;
	gbye = newwin(3, 12, (W_HEIGHT - 3)/2 + 1, (W_WIDTH - 12)/2 + 1);
	wattron(gbye, A_BOLD);
	box(gbye, 0, 0);
	if (cal_progress() == 27)
		mvwprintw(gbye, 1,2, "You win!");
	else
		mvwprintw(gbye, 1,2, "Goodbye!");
	wattroff(gbye, A_BOLD);
	wgetch(gbye);
}

