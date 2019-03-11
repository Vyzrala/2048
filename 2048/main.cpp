#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include <time.h>
#include <cstdlib>

extern "C" {
#include"./sdl-2.0.7/include/SDL.h"
#include"./sdl-2.0.7/include/SDL_main.h"
}

#define POSITION_X 200 
#define POSITION_Y 200 
#define MENU_X 13
#define MENU_Y 140
#define SHIFT 128
#define ROZMIAR 4
#define SCREEN_WIDTH	SHIFT*(ROZMIAR+2)
#define SCREEN_HEIGHT	SHIFT*(ROZMIAR+2)

int ROZMIARA;

// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};


// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
};


// rysowanie prostok¹ta o d³ugoœci boków l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};

// main
#ifdef __cplusplus
extern "C"
#endif

struct pole {
	char token = NULL;
	int value = NULL;
	SDL_Surface *bitmap = NULL;
	int x = NULL;
	int y = NULL;
};

void set_pole(pole *field,char token, int value, SDL_Surface *bitmapa,int x_pos,int y_pos) {
	// @ = puste, A = 2, B = 4, C = 8, D = 16, E = 32, F = 64, G = 128, H = 256, I = 512, J = 1024, K = 2048
	field->token = token; 
	field->value = value;
	field->bitmap = bitmapa;
	field->x = x_pos;
	field->y = y_pos;
}

void Draw_board(SDL_Surface *screen,pole **plansza) {
	for (int i = 0; i < ROZMIAR; i++) 
		for (int j = 0; j < ROZMIAR; j++) 
			DrawSurface(screen, plansza[i][j].bitmap, POSITION_X + plansza[i][j].x * SHIFT,POSITION_Y +plansza[i][j].y*SHIFT);
}

int losowa(int a) {
	int m = rand() % a;
	return m;
}

void add_new(pole **plansza) {

	int x = losowa(ROZMIAR);
	int y = losowa(ROZMIAR);
	int m = losowa(100);
	if (plansza[x][y].value == 0) {
		if(m<91) plansza[x][y].value = 2; // 90 % possibility for 2
		else plansza[x][y].value = 4; // 10 % possibility for 4
	}
	else add_new(plansza);
}

void zeruj(pole *field) {
	field->token = '@';
	field->value = 0;
	field->bitmap = NULL;
	field->x = NULL;
	field->y = NULL;
}

void new_game(pole **plansza) {
	for (int i = 0; i < ROZMIAR; i++)
		for (int j = 0; j < ROZMIAR; j++)
			zeruj(&plansza[i][j]);
}

bool czy_win(pole** plansza) {
	for (int i = 0; i < ROZMIAR; i++) 
		for (int j = 0; j < ROZMIAR; j++) 
			if (plansza[i][j].value == 2048) return true;

	return false;
}

void points(int value, int* score) {
	// 2 = +0pts = 0x2^1, 2^1 = 2
	// 4 = +4pts = 1x2^2, 2^2 = 4
	// 8 = +16pts = 2x2^3, 2^3 = 8
	// 16 = +48pts = 3x2^4, 2^4 = 16 
	// ...
	switch (value) { // case 2 = 0, so i did not consider this case
	case 4:
		*score += 1 * value;
		break;
	case 8:
		*score += 2 * value;
		break;
	case 16:
		*score += 3 * value;
		break;
	case 32:
		*score += 4 * value;
		break;
	case 64:
		*score += 5 * value;
		break;
	case 128:
		*score += 6 * value;
		break;
	case 256:
		*score += 7 * value;
		break;
	case 512:
		*score += 8 * value;
		break;
	case 1024:
		*score += 9 * value;
		break;
	case 2048:
		*score += 10 * value;
		break;
	}
}

void move(pole ** plansza, SDL_Keycode key) {
	for (int k = 0; k < ROZMIAR; k++) {
		for (int i = 0; i < ROZMIAR; i++) { // moving towards walls
			if (key == SDLK_UP) {
				for (int j = 0; j < ROZMIAR - 1; j++) {
					if (plansza[i][j].token == '@') {
						plansza[i][j] = plansza[i][j + 1];
						zeruj(&plansza[i][j + 1]);
					}
				}
			}
			else if (key == SDLK_DOWN) {
				for (int j = ROZMIAR - 1; j > 0; j--) {
					if (plansza[i][j].token == '@') {
						plansza[i][j] = plansza[i][j - 1];
						zeruj(&plansza[i][j - 1]);
					}
				}
			}
			else if (key == SDLK_RIGHT) {
				for (int j = ROZMIAR - 1; j > 0; j--) {
					if (plansza[j][i].token == '@') {
						plansza[j][i] = plansza[j - 1][i];
						zeruj(&plansza[j - 1][i]);
					}
				}
			}
			else if (key == SDLK_LEFT) {
				for (int j = 0; j < ROZMIAR - 1; j++) {
					if (plansza[j][i].token == '@') {
						plansza[j][i] = plansza[j + 1][i];
						zeruj(&plansza[j + 1][i]);
					}
				}
			}
		}
	}
}

void merge(pole **plansza, SDL_Keycode key, int *score) {
	switch (key) {
	case SDLK_UP:
		for (int i = 0; i < ROZMIAR; i++) {
			for (int j = 0; j < ROZMIAR - 1; j++) {
				if (plansza[i][j].value == plansza[i][j + 1].value) {
					plansza[i][j].value *= 2;
					zeruj(&plansza[i][j + 1]);
					*score += plansza[i][j].value;
					//points(plansza[i][j].value, score);
				}
			}
		}
		break;
	case SDLK_DOWN:
		for (int i = 0; i < ROZMIAR; i++) {
			for (int j = ROZMIAR - 1; j > 0; j--) {
				if (plansza[i][j].value == plansza[i][j - 1].value) {
					plansza[i][j].value *= 2;
					zeruj(&plansza[i][j - 1]);
					*score += plansza[i][j].value;
					//points(plansza[i][j].value, score);
				}
			}
		}
		break;
	case SDLK_RIGHT:
		for (int i = 0; i < ROZMIAR; i++) {
			for (int j = ROZMIAR - 1; j > 0; j--) {
				if (plansza[j][i].value == plansza[j - 1][i].value) {
					plansza[j][i].value *= 2;
					zeruj(&plansza[j - 1][i]);
					*score += plansza[j][i].value;
					//points(plansza[j][i].value, score);
				}
			}
		}
		break;
	case SDLK_LEFT:
		for (int i = 0; i < ROZMIAR; i++) {
			for (int j = 0; j < ROZMIAR - 1; j++) {
				if (plansza[j][i].value == plansza[j + 1][i].value) {
					plansza[j][i].value *= 2;
					zeruj(&plansza[j + 1][i]);
					*score += plansza[j][i].value;
					//points(plansza[j][i].value, score);
				}
			}
		}
		break;
	}
}

void ruch(pole **plansza, SDL_Keycode key, int *score) {
	move(plansza, key);
	merge(plansza, key, score);

	move(plansza, key);
}

bool sasiad(pole **plansza) {
	for (int i = 0; i < ROZMIAR; i++) {
		for (int j = 0; j < ROZMIAR; j++) {
			if (i == 0) {
				if (plansza[i][j].token == plansza[i + 1][j].token) return true;
			}
			else if (i == ROZMIAR - 1) {
				if (plansza[i][j].token == plansza[i - 1][j].token) return true;
			}
			else {
				if (plansza[i][j].token == plansza[i + 1][j].token || 
					plansza[i][j].token == plansza[i - 1][j].token) return true;
			}
		}
	}
	for (int i = 0; i < ROZMIAR; i++) {
		for (int j = 0; j < ROZMIAR; j++) {
			if (j == 0) {
				if (plansza[i][j].token == plansza[i][j + 1].token) return true;
			}
			else if (j == ROZMIAR - 1) {
				if (plansza[i][j].token == plansza[i][j - 1].token) return true;
			}
			else {
				if (plansza[i][j].token == plansza[i][j + 1].token ||
					plansza[i][j].token == plansza[i][j - 1].token) return true;
			}
		}
	}
	return false;
}

void MENU(SDL_Surface *screen, SDL_Surface *charset, char *text, int czerwony, int niebieski, int*score) {
	int jump = 12;
	int step = 0;
	int *s = &step;

	// okno punktacji
	DrawRectangle(screen, MENU_X-5, 70, 140, 40, czerwony, niebieski);
	sprintf(text, "Points = %d", *score);
	DrawString(screen, MENU_X, 80, text, charset);
	sprintf(text, "Rozmiar: %dx%d", ROZMIAR, ROZMIAR);
	DrawString(screen, MENU_X, 93, text, charset);
	
	// okno menu
	DrawRectangle(screen, MENU_X-5, MENU_Y-5, 120, 140, czerwony, niebieski);
	sprintf(text, "Marcin");
	DrawString(screen, MENU_X, MENU_Y + ((*s)++) * jump, text, charset);
	sprintf(text, "...");
	DrawString(screen, MENU_X, MENU_Y + ((*s)++) * jump, text, charset);
	sprintf(text, "...");
	DrawString(screen, MENU_X, MENU_Y + ((*s)++) * jump, text, charset);
	sprintf(text, "----------");
	DrawString(screen, MENU_X, MENU_Y + ((*s)++) * jump, text, charset);
	sprintf(text, "MENU: ");
	DrawString(screen, MENU_X, MENU_Y + ((*s)++) * jump, text, charset);
	(*s)++;
	sprintf(text, "ESC = Quit ");
	DrawString(screen, MENU_X, MENU_Y + ((*s)++) * jump, text, charset);
	sprintf(text, "N = New game");
	DrawString(screen, MENU_X, MENU_Y + ((*s)++) * jump, text, charset);
	sprintf(text, "U = Undo");
	DrawString(screen, MENU_X, MENU_Y + ((*s)++) * jump, text, charset);


}

int main(int argc, char **argv) {
	int t1, t2, quit, frames, rc;
	double delta, worldTime, fpsTimer, fps, distance, etiSpeed;
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Surface *eti;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Keycode key;
	
	srand(time(NULL));

	int liczba_wolnych = 0;
	bool enable_move = true;
	bool win = false;
	bool first_move = true;
	bool neighbor;
	int score = 0;
	int score_undo = 0;

	//scanf("%d", &ROZMIARA);


	
	pole **plansza = new pole*[ROZMIAR];
	pole **undo = new pole*[ROZMIAR];

	for (int i = 0; i < ROZMIAR; i++) {
		plansza[i] = new pole[ROZMIAR];
		undo[i] = new pole[ROZMIAR];
	}

	SDL_Surface *points;
	// loading bitmaps
	points = SDL_LoadBMP("./bitmaps/points.bmp");

	SDL_Surface *empty, *_2, *_4, *_8, *_16, *_32, *_64, *_128, *_256, *_512, *_1024, *_2048;
	empty = SDL_LoadBMP("./bitmaps/empty.bmp");
	_2 = SDL_LoadBMP("./bitmaps/2.bmp");
	_4 = SDL_LoadBMP("./bitmaps/4.bmp");
	_8 = SDL_LoadBMP("./bitmaps/8.bmp");
	_16 = SDL_LoadBMP("./bitmaps/16.bmp");
	_32 = SDL_LoadBMP("./bitmaps/32.bmp");
	_64 = SDL_LoadBMP("./bitmaps/64.bmp");
	_128 = SDL_LoadBMP("./bitmaps/128.bmp");
	_256 = SDL_LoadBMP("./bitmaps/256.bmp");
	_512 = SDL_LoadBMP("./bitmaps/512.bmp");
	_1024 = SDL_LoadBMP("./bitmaps/1024.bmp");
	_2048 = SDL_LoadBMP("./bitmaps/2048.bmp");
	

	// okno konsoli nie jest widoczne, je¿eli chcemy zobaczyæ
	// komunikaty wypisywane printf-em trzeba w opcjach:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// zmieniæ na "Console"
	// console window is not visible, to see the printf output
	// the option:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// must be changed to "Console"
	printf("wyjscie printfa trafia do tego okienka\n");
	printf("printf output goes here\n");

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	// tryb pe³noekranowy / fullscreen mode
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                 &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Marcin Hebdzynski, 171608");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
	                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset, true, 0x000000);

	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;
	etiSpeed = 1;

	while(!quit) {
		t2 = SDL_GetTicks();

		// w tym momencie t2-t1 to czas w milisekundach,
		// jaki uplyna³ od ostatniego narysowania ekranu
		// delta to ten sam czas w sekundach
		// here t2-t1 is the time in milliseconds since
		// the last screen was drawn
		// delta is the same time in seconds
		delta = (t2 - t1) * 0.001;
		t1 = t2;

		worldTime += delta;

		distance += etiSpeed * delta;

		SDL_FillRect(screen, NULL, czarny);

		if (first_move) { // zeby na poczatku byl jeden klocek
			add_new(plansza);
			first_move = false;
		}

		// update planszy
		//set_plansza(plansza);
		for (int i = 0; i < ROZMIAR; i++) {
			for (int j = 0; j < ROZMIAR; j++) {
				switch (plansza[i][j].value) {
				case 0:
					set_pole(&plansza[i][j], '@', plansza[i][j].value, empty, i, j);
					break;
				case 2:
					set_pole(&plansza[i][j], 'A', plansza[i][j].value, _2, i, j);
					break;
				case 4:
					set_pole(&plansza[i][j], 'B', plansza[i][j].value, _4, i, j);
					break;
				case 8:
					set_pole(&plansza[i][j], 'C', plansza[i][j].value, _8, i, j);
					break;
				case 16:
					set_pole(&plansza[i][j], 'D', plansza[i][j].value, _16, i, j);
					break;
				case 32:
					set_pole(&plansza[i][j], 'E', plansza[i][j].value, _32, i, j);
					break;
				case 64:
					set_pole(&plansza[i][j], 'F', plansza[i][j].value, _64, i, j);
					break;
				case 128:
					set_pole(&plansza[i][j], 'G', plansza[i][j].value, _128, i, j);
					break;
				case 256:
					set_pole(&plansza[i][j], 'H', plansza[i][j].value, _256, i, j);
					break;
				case 512:
					set_pole(&plansza[i][j], 'I', plansza[i][j].value, _512, i, j);
					break;
				case 1024:
					set_pole(&plansza[i][j], 'J', plansza[i][j].value, _1024, i, j);
					break;
				case 2048:
					set_pole(&plansza[i][j], 'K', plansza[i][j].value, _2048, i, j);
					break;
				}
			}
		}
		
		MENU(screen, charset, text, czerwony, niebieski, &score);

		Draw_board(screen, plansza);

		// show pattern of counting score
		//DrawSurface(screen, points, 70, SHIFT*2+ 90);

		neighbor = sasiad(plansza);
		if (neighbor) enable_move = true;

		liczba_wolnych = 0;

		for (int i = 0; i < ROZMIAR; i++)
			for (int j = 0; j < ROZMIAR; j++)
				if (plansza[i][j].token == '@')
					liczba_wolnych++;


		win = czy_win(plansza);

		if (liczba_wolnych == 0 && !win && !neighbor) {
			Draw_board(screen, plansza);
			DrawRectangle(screen, SCREEN_HEIGHT / 2 - SHIFT, SHIFT / 2.5, 200, 80, czerwony, niebieski);
			sprintf(text, "Game over");
			DrawString(screen, SCREEN_HEIGHT / 2 - SHIFT / 2, SHIFT / (1.6), text, charset);
			sprintf(text, "Your score: %d", score);
			DrawString(screen, SCREEN_HEIGHT / 2 - SHIFT / 2 - 25, SHIFT / 1.4, text, charset);
			enable_move = false;
		}

		if (win) {
			Draw_board(screen, plansza);
			DrawRectangle(screen, SCREEN_HEIGHT / 2 - SHIFT, SHIFT / 2.5, 200, 80, czerwony, niebieski);
			sprintf(text, "You won!");
			DrawString(screen, SCREEN_HEIGHT / 2 - SHIFT / 2, SHIFT / (1.6), text, charset);
			sprintf(text, "Your score: %d", score);
			DrawString(screen, SCREEN_HEIGHT / 2 - SHIFT / 2 - 25, SHIFT / 1.4, text, charset);
			enable_move = false;
		}

		fpsTimer += delta;
		if(fpsTimer > 0.5) {
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
		};

		// tekst informacyjny / info text
		DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
		//            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"
		sprintf(text, "Marcin Hebdzynski 171608. Czas trwania = %.1lf s  %.0lf klatek / s", worldTime, fps);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
		//	      "Esc - exit, \030 - faster, \031 - slower"
		sprintf(text, "Esc - wyjscie, \030 - przyspieszenie, \031 - zwolnienie");
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
//		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		// obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)

		while(SDL_PollEvent(&event)) {
			
			switch(event.type) {
				case SDL_KEYDOWN:
					key = event.key.keysym.sym;
					if (key == SDLK_ESCAPE) quit = 1;
					else if (key == SDLK_UP || key == SDLK_DOWN || key == SDLK_LEFT || key == SDLK_RIGHT) { // make move
						for (int i = 0; i < ROZMIAR; i++) { // saving board to be able to make undo
							for (int j = 0; j < ROZMIAR; j++) {
								undo[i][j] = plansza[i][j];
							}
						}
						score_undo = score;
						if (enable_move) { 
							ruch(plansza, key,&score);
							if(liczba_wolnych>0) add_new(plansza);
						}
					}
					else if (key == SDLK_n) { // new game
						new_game(plansza);
						worldTime = 0;
						win = false;
						liczba_wolnych = 0;
						score = 0;
						enable_move = true;
						first_move = true;
					}
					else if (key == SDLK_u) { // undo
						enable_move = true;
						score = score_undo;
						for (int i = 0; i < ROZMIAR; i++)
							for (int j = 0; j < ROZMIAR; j++)
								plansza[i][j] = undo[i][j];
					}
					break;
				case SDL_KEYUP:
					etiSpeed = 1.0;
					break;
				case SDL_QUIT:
					quit = 1;
					break;
			};
		};
		frames++;
	};
	
	for (int i = 0; i < ROZMIAR; i++) {
		delete[] plansza[i];
		delete[] undo[i];
	}
	delete[] plansza, undo;


	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
	};
