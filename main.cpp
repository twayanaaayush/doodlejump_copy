#include <iostream>
#include "SDL.h"
#include "SDL_image.h"
#include <ctime>

#define WINDOWWIDTH 400
#define WINDOWHEIGHT 533

static SDL_Window* window;
static SDL_Renderer* renderer;
SDL_Event event;

SDL_Texture* sBg, * sPlatform, * sPlayer;
static SDL_Rect bRect, pRect, plRect[10];
static SDL_RendererFlip spriteFlip = SDL_FLIP_NONE;

//////////////////////////some variables/////////////////////////
static bool isRunning = true;
const static int maxYPos = 200;

/////////////////////////////function declarations///////////////////////////
void init();
void handleEvents();
void update();
SDL_Texture* LoadTexture(const char*);
void Draw(SDL_Texture*, SDL_Rect, int);
void render();
void clean();

/////////////////////////basic player and platform struct///////////////////////
struct PlayerStruct { float x, y, dx, dy, dir, speed; };
struct PlatformStruct { float x, y; };

PlayerStruct player;
PlatformStruct platform[10];

/////////////////////////////function definations///////////////////////////
int main(int argc, char* argv[]) {

	srand(time(0));
	init();

	while(isRunning) {
		handleEvents();
		update();
		render();
	}

	if(!isRunning){
		clean();
	}
	return 0;
}

void init() {
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Doodle Jump", SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED, WINDOWWIDTH, WINDOWHEIGHT, 0);

	renderer = SDL_CreateRenderer(window, -1, 0);

//////////////////////Load Textures From File//////////////////////////
	sBg = LoadTexture("assets/background.PNG");
	sPlatform = LoadTexture("assets/platform.PNG");
	sPlayer = LoadTexture("assets/doodle.PNG");

//////////////////////Initialize the Struct Members/////////////////////
	player.speed = 0.2f;
	player.x = 170;
	player.y = 300;
	player.dx = player.dy = 0;

//////////////////////Initialize the player Rect///////////////////////////
	pRect.x = (int)player.x;
	pRect.y = (int)player.y;
	pRect.w = 50;
	pRect.h = 70;

///////////////////////////Initialize the Bg Rect///////////////////////
	bRect = {0, 0, 400, 550};

/////////////////////////////Initialize the platforms/////////////////////////
	for(int i = 0; i < 10; i++) {
		platform[i]. x = (rand() % WINDOWWIDTH) + 1;
		platform[i]. y = (rand() % WINDOWHEIGHT) + 1;
		//////////////Initialize the platform rect//////////////////
		plRect[i].x = (int)platform[i].x;
		plRect[i].y = (int)platform[i].y;
		plRect[i].w = 100;
		plRect[i].h = 20;
	}
}

void handleEvents() {
	SDL_PollEvent(&event);
	switch(event.type) {
	case SDL_QUIT:
		isRunning = false;
		break;

	case SDL_KEYDOWN:
		switch(event.key.keysym.sym) {
		case SDLK_LEFT:
			player.dir = -1.0f;
			spriteFlip = SDL_FLIP_NONE;
			break;
		case SDLK_RIGHT:
			player.dir = 1.0f;
			spriteFlip = SDL_FLIP_HORIZONTAL;
			break;

		default:
			break;
		}
		break;

	case SDL_KEYUP:
		switch(event.key.keysym.sym) {
		case SDLK_LEFT:
			player.dir = 0.0f;
			break;
		case SDLK_RIGHT:
			player.dir = 0.0f;
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}
}

void update() {

/////////////updating the position as per the input////////////////
	player.x += player.dir * player.speed;
	pRect.x = (int)player.x;

//////////////////////////////gravity///////////////////////////
	player.dy += 0.0002;
	player.y += player.dy;

//	std::cout << player.dy << " " << player.y << std::endl;

/////////////if player is out of bound spring it back into screen/////////
	if(player.y > 500) player.dy = -0.2;
	pRect.y = player.y;

	if(player.x > WINDOWWIDTH) player.x = -(WINDOWWIDTH - player.x);
	if(player.x < 0) player.x = (WINDOWWIDTH - player.x);

////////////////////Scrolling effect movement/////////////////////////
	if(player.y < maxYPos) {
		for(int i = 0; i < 10; i++) {
			player.y = maxYPos - player.dy;
			platform[i].y = platform[i].y - player.dy;

			if(platform[i].y > WINDOWHEIGHT) {
				platform[i].y = 0;
				platform[i].x = (rand() % WINDOWWIDTH) + 1;
			}

			plRect[i].x = (int)platform[i].x;
			plRect[i].y = (int)platform[i].y;
		}
	}

///////////////////collision detection////////////////////////
	for(int i = 0; i < 10; i++) {
		if(
			(player.x + pRect.w > platform[i].x) &&
			(player.x + (pRect.w / 2) < platform[i].x + plRect[i].w) &&
			(player.y + pRect.h < platform[i].y + plRect[i].h) &&
			(player.y + pRect.h > platform[i].y) &&
			(player.dy > 0)
		) player.dy -= 0.4;
	}

}

void render() {
	SDL_RenderClear(renderer);

	Draw(sBg, bRect, 0);
	Draw(sPlayer, pRect, 1);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//	SDL_RenderDrawRect(renderer, &pRect);

	for(int i = 0; i < 10; i++) {
		Draw(sPlatform, plRect[i], 0);
//		SDL_RenderDrawRect(renderer, &plRect[i]);
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderPresent(renderer);

}

SDL_Texture* LoadTexture(const char* fileName) {

	SDL_Surface* tempSurface = IMG_Load(fileName);
	SDL_Texture* tempTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
	SDL_FreeSurface(tempSurface);

	return tempTexture;
}

void Draw(SDL_Texture* texture, SDL_Rect dest, int flip) {
	if(flip == 0) {
		SDL_RenderCopyEx(renderer, texture, NULL, &dest, NULL, NULL, SDL_FLIP_NONE);
	} else if (flip == 1){
		SDL_RenderCopyEx(renderer, texture, NULL, &dest, NULL, NULL, spriteFlip);
	}
}

void clean() {
	SDL_DestroyTexture(sBg);
	SDL_DestroyTexture(sPlatform);
	SDL_DestroyTexture(sPlayer);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}
