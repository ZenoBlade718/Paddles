#include <SDL2/SDL.h>
#include <random>
#include <math.h>
#include <time.h>
#define PI2 2*M_PI
SDL_Window* AppWindow = NULL;SDL_Renderer* AppRenderer = NULL;const short Move = 10;
SDL_Event event;const short SCREEN_WIDTH = 720, SCREEN_HEIGHT = 600;
int PlayerScore[2];bool GameOver = false;
enum PLAYERS{
    PLAYER_1,
    PLAYER_2,
    BALL
};

void AppClose(){
    if(AppRenderer != NULL)  SDL_DestroyRenderer(AppRenderer);AppRenderer = NULL;
    if(AppWindow != NULL)SDL_DestroyWindow(AppWindow);AppWindow = NULL;
    SDL_Quit();
}
bool AppInit(){
    bool c = true;
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("SDL not initialised: %s\n", SDL_GetError());c = false;
    }
    else{
        AppWindow = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_BORDERLESS);
        if(AppWindow == NULL){
            printf("Window not created: %s\n" , SDL_GetError());c = false;
        }
        else{
            AppRenderer = SDL_CreateRenderer(AppWindow, -1, SDL_RENDERER_ACCELERATED);
            if(AppRenderer == NULL){
                printf("Renderer not created: %s\n" , SDL_GetError());c = false;                
            }
        }
    }
    return c;
}

//Players 1 and 2 as well as the Ball
SDL_Rect Players[3];
void Player1Movement(){
    const Uint8* KeyState = SDL_GetKeyboardState(NULL);
//Each combination needs to be handled seperatly. IDK how else to handle simultaneuos input
    if(KeyState[SDL_SCANCODE_UP]){
        if(Players[PLAYER_1].y > 0)Players[PLAYER_1].y -= Move;
        else if(Players[PLAYER_1].y < 0) Players[PLAYER_1].y = 0;
    }
    else if(KeyState[SDL_SCANCODE_DOWN]){
        if((Players[PLAYER_1].y + Players[PLAYER_1].h) < SCREEN_HEIGHT)Players[PLAYER_1].y += Move;
        else if((Players[PLAYER_1].y + Players[PLAYER_1].h) > SCREEN_HEIGHT) Players[PLAYER_1].y = SCREEN_HEIGHT - Players[PLAYER_1].h;
    }
    else if((KeyState[SDL_SCANCODE_W] || KeyState[SDL_SCANCODE_S]) && KeyState[SDL_SCANCODE_UP]){
        if(Players[PLAYER_1].y > 0)Players[PLAYER_1].y -= Move;
        else if(Players[PLAYER_1].y < 0) Players[PLAYER_1].y = 0;
    }
    else if((KeyState[SDL_SCANCODE_W] || KeyState[SDL_SCANCODE_S]) && KeyState[SDL_SCANCODE_DOWN]){
        if((Players[PLAYER_1].y + Players[PLAYER_1].h) < SCREEN_HEIGHT)Players[PLAYER_1].y += Move;
        else if((Players[PLAYER_1].y + Players[PLAYER_1].h) > SCREEN_HEIGHT) Players[PLAYER_1].y = SCREEN_HEIGHT - Players[PLAYER_1].h;
    }
}

void Player2Movement(){
    const Uint8* KeyState = SDL_GetKeyboardState(NULL);
    //Each combination needs to be handled seperatly. IDK how else to handle simultaneuos input
    if(KeyState[SDL_SCANCODE_W]){
        if(Players[PLAYER_2].y > 0)Players[PLAYER_2].y -= Move;
        else if(Players[PLAYER_2].y < 0) Players[PLAYER_2].y = 0;
    }
    else if(KeyState[SDL_SCANCODE_S]){
        if((Players[PLAYER_2].y + Players[PLAYER_2].h) < SCREEN_HEIGHT)Players[PLAYER_2].y += Move;
        else if((Players[PLAYER_2].y + Players[PLAYER_2].h) > SCREEN_HEIGHT) Players[PLAYER_2].y = SCREEN_HEIGHT - Players[PLAYER_2].h;
    }
    else if((KeyState[SDL_SCANCODE_UP] || KeyState[SDL_SCANCODE_DOWN]) && KeyState[SDL_SCANCODE_W]){
        if(Players[PLAYER_2].y > 0)Players[PLAYER_2].y -= Move;
        else if(Players[PLAYER_2].y < 0) Players[PLAYER_2].y = 0;
    }
    else if((KeyState[SDL_SCANCODE_UP] || KeyState[SDL_SCANCODE_DOWN]) && KeyState[SDL_SCANCODE_S]){
        if((Players[PLAYER_2].y + Players[PLAYER_2].h) < SCREEN_HEIGHT)Players[PLAYER_2].y += Move;
        else if((Players[PLAYER_2].y + Players[PLAYER_2].h) > SCREEN_HEIGHT) Players[PLAYER_2].y = SCREEN_HEIGHT - Players[PLAYER_2].h;
    }
}

int Ball(double &direction){
    short velocity = 10;
    if((Players[BALL].y <= 0) || 
       ((Players[BALL].y + Players[BALL].h) >= SCREEN_HEIGHT) ){
         direction = -direction;
    }
    //Collision with right paddle
    else if((Players[BALL].y <= (Players[PLAYER_2].y+Players[PLAYER_2].h)) && 
            ((Players[BALL].y + Players[BALL].h) >= (Players[PLAYER_2].y)) && 
            ((Players[BALL].x + Players[BALL].w) >= Players[PLAYER_2].x) &&
            (Players[BALL].x <= (Players[PLAYER_2].x + Players[PLAYER_2].w))){
	    direction = M_PI - direction;
	    Players[BALL].x = Players[PLAYER_2].x - Players[BALL].w - 1;

    }
    //Collision with left paddle
    else if((Players[BALL].y <= (Players[PLAYER_1].y+Players[PLAYER_1].h)) && 
            ((Players[BALL].y + Players[BALL].h) >= (Players[PLAYER_1].y)) && 
            ((Players[PLAYER_1].x + Players[PLAYER_1].w) >= Players[BALL].x) &&
            (Players[BALL].x <= Players[PLAYER_1].x + Players[PLAYER_1].w)){
	    direction = M_PI - direction;
	    Players[BALL].x = Players[PLAYER_1].x + Players[BALL].w +2;
    }
    else if(Players[BALL].x <= 0){
        GameOver = true;PlayerScore[1] += 1;
    }
    else if(Players[BALL].x >= SCREEN_WIDTH){
        GameOver = true;PlayerScore[0] += 1;
    }
    double new_x = (double)velocity * cos(direction);
    double new_y = (double)velocity * sin(direction);
    Players[BALL].y += new_y;
    Players[BALL].x += new_x;
    return direction;
}

int main(){
    //Cus I don't like using !AppInit()s
    if(AppInit() == false){
        printf("Something went wrong in AppInit: %s\n" , SDL_GetError());
        return 1;
    }
    else{
        //Other variables to use in the game loop :P
        PlayerScore[0] = 0, PlayerScore[1] = 0;
        Players[PLAYER_1].h = 100, Players[PLAYER_1].w = 20, Players[PLAYER_1].x = 20, Players[PLAYER_1].y = 200;
        Players[PLAYER_2].h = 100, Players[PLAYER_2].w = 20, Players[PLAYER_2].x = 680, Players[PLAYER_2].y = 200;
        Players[BALL].h = 20, Players[BALL].w =20, Players[BALL].x = 340,Players[BALL].y=270;
        double direction = 0;
        time_t tim = 0;
        time(&tim);srand(tim);
        direction = ((double)rand() / RAND_MAX) * PI2;
        bool quit = false;
        while (quit == false && GameOver == false)
        {
            SDL_SetRenderDrawColor(AppRenderer, 0, 0, 0, 255);
            SDL_RenderClear(AppRenderer);
            while(SDL_PollEvent(&event) != 0){
            //switch in case other events need to be processed(I'm glad for this after making it)
                switch(event.type){
                    //Two ways to exit for convinience
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.scancode == SDL_SCANCODE_Q){
                        quit = true;
                    }
                    break;
                }
            }
            direction = Ball(direction);
            Player1Movement();
            Player2Movement();
            if(SDL_RenderDrawRects(AppRenderer, Players, 3) != 0){
                printf("Players weren't drawn: %s\n", SDL_GetError());
            }
            else{
                SDL_SetRenderDrawColor(AppRenderer, 255, 255, 255, 255);
                SDL_RenderFillRects(AppRenderer, Players, 3);
            }
            SDL_RenderPresent(AppRenderer);
            SDL_Delay(50);
        }
        printf("Player 1 Score: %d", PlayerScore[0]);printf("\n");
        printf("Player 2 Score: %d", PlayerScore[1]);printf("\n");
    }
    AppClose();
    return 0;
}
