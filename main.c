#include <stdio.h>
#include <SDL2/SDL.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>

/* neighborhood mode */
#define MOORE
#define NEWMAN

//Screen dimension constants
#define SCREEN_WIDTH    1200
#define SCREEN_HEIGHT   850

#define MAX_NEIGHBORS 8   /* Max number of neighbors */

typedef uint32_t rgba;
typedef int32_t  coor;
typedef uint8_t  color;

/* sizeof == 1 */
typedef struct{
    unsigned int alive;
    unsigned int inner; // 8
    unsigned int outer; // 16
}neighborhood;

/* Default rule sets */
/* Born */
bool B[9] = {
/* 0 */ 0,
/* 1 */ 0,
/* 2 */ 0,
/* 3 */ 1,
/* 4 */ 0,
/* 5 */ 0,
/* 6 */ 1,
/* 7 */ 1,
/* 8 */ 1,
};

/* Survives */
bool S[9] = {
/* 0 */ 0,
/* 1 */ 0,
/* 2 */ 0,
/* 3 */ 1,
/* 4 */ 1,
/* 5 */ 0,
/* 6 */ 1,
/* 7 */ 1,
/* 8 */ 1,
};

void print_ruleset()
{
    int i;
    
    printf("B");
    for (i = 0; i < 9; ++i)
        if(B[i])
            printf("%d", i);
    printf("/S");
    for (i = 0; i < 9; ++i)
        if(S[i])
            printf("%d", i);
    printf("\n");
}

/* Gets an index for the board array */
static inline rgba _get_index(coor x, coor y)
{
    if (x < 0)
        x = SCREEN_WIDTH + x;
    else
        x = x % SCREEN_WIDTH;
    if (y < 0)
        y = SCREEN_HEIGHT + y;
    else
        y = y % SCREEN_HEIGHT;
    return (SCREEN_WIDTH * y) + x;
}

void set_pixel(SDL_Surface *surface, coor x, coor y, rgba color_alpha)
{
    SDL_PixelFormat *fmt;
    fmt = surface->format;
    rgba* pixel = surface->pixels;
    unsigned int index = _get_index(x, y);
    
    SDL_LockSurface(surface);
    pixel[index] = color_alpha;
    SDL_UnlockSurface(surface);
}

rgba get_pixel(SDL_Surface *surface, coor x, coor y)
{
    SDL_PixelFormat *fmt;
    fmt = surface->format;
    rgba* pixel = surface->pixels;
    
    return pixel[_get_index(x, y)]&0x00FFFFFF;
}

neighborhood get_neighbors(rgba* surface, coor x, coor y)
{
    neighborhood neighbors;
    
    neighbors.inner = 0;
    neighbors.outer = 0;
    neighbors.alive = 0;
    
    /* inner neighborhood */
    if (surface[_get_index(x, y-1)])    neighbors.inner++;
    if (surface[_get_index(x, y+1)])    neighbors.inner++;
    if (surface[_get_index(x-1, y)])    neighbors.inner++;
    if (surface[_get_index(x-1, y-1)])  neighbors.inner++;
    
    if (surface[_get_index(x-1, y+1)])  neighbors.inner++;
    if (surface[_get_index(x+1, y)])    neighbors.inner++;
    if (surface[_get_index(x+1, y-1)])  neighbors.inner++;
    if (surface[_get_index(x+1, y+1)])  neighbors.inner++;
    
    /* outer neighborhood */
//    if (surface[_get_index(x, y)])    ++neighbors.outer;
//    if (surface[_get_index(x, y)])    ++neighbors.outer;
//    if (surface[_get_index(x, y)])    ++neighbors.outer;
//    if (surface[_get_index(x, y)])    ++neighbors.outer;
//
//    if (surface[_get_index(x, y)])    ++neighbors.outer;
//    if (surface[_get_index(x, y)])    ++neighbors.outer;
//    if (surface[_get_index(x, y)])    ++neighbors.outer;
//    if (surface[_get_index(x, y)])    ++neighbors.outer;
//
//    if (surface[_get_index(x, y)])    ++neighbors.outer;
//    if (surface[_get_index(x, y)])    ++neighbors.outer;
//    if (surface[_get_index(x, y)])    ++neighbors.outer;
//    if (surface[_get_index(x, y)])    ++neighbors.outer;
//
//    if (surface[_get_index(x, y)])    ++neighbors.outer;
//    if (surface[_get_index(x, y)])    ++neighbors.outer;
//    if (surface[_get_index(x, y)])    ++neighbors.outer;
//    if (surface[_get_index(x, y)])    ++neighbors.outer;

    /* is the current cell alive? */
    if (surface[_get_index(x, y)])      neighbors.alive = 1;
    
    return neighbors;
}

/* rules for the game of life */
bool is_alive(neighborhood neighbors)
{
    if (neighbors.alive)
    {
        if (S[neighbors.inner])
            neighbors.alive = true;
        else
            neighbors.alive = false;
    }
    else
    {
        if (B[neighbors.inner])
            neighbors.alive = true;
        else
            neighbors.alive = false;
    }
    if (neighbors.alive)
        return true;
    else
        return false;
}

bool get_bool()
{
        if (rand()%2)
            return true;
        else
            return false;
}

int main( int argc, char* args[] )
{
    /* indexes */
    unsigned int i, j;
    SDL_Event Events;
    bool running = true;
    bool pause = false;
    bool show_heat = true;
    bool show_life = true;
    signed int speed = 17;
    
    /* temp board */
    rgba* temp_board = (rgba*) malloc( SCREEN_WIDTH * SCREEN_WIDTH * sizeof(rgba));
    rgba* cur_board  = (rgba*) malloc( SCREEN_WIDTH * SCREEN_WIDTH * sizeof(rgba));
    rgba* heat_map   = (rgba*) calloc( SCREEN_WIDTH * SCREEN_WIDTH, sizeof(rgba));

    rgba* swappy = NULL;

    //The window we'll be rendering to
    SDL_Window* window = NULL;
    
    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
    else
    {
        //Create window
        window = SDL_CreateWindow(
            "Game of Life",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN);
    
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get window surface
            screenSurface = SDL_GetWindowSurface( window );
            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x0, 0x0, 0x0 ) );
        
            /* Set random */
            for (i = 0; i < SCREEN_HEIGHT; ++i)
                for (j = 0; j < SCREEN_WIDTH; ++j)
                    temp_board[_get_index(j, i)] = get_bool();

            /* run the 'game' */
            while (running)
            {
                if (!pause)
                {
                    /* update with current alive status */
                    for (i = 0; i < SCREEN_HEIGHT; ++i)
                        for (j = 0; j < SCREEN_WIDTH; ++j)
                            cur_board[_get_index(j, i)] = is_alive(get_neighbors(temp_board, j, i));
                
                    /* draw game */
                    for (i = 0; i < SCREEN_HEIGHT; ++i)
                    {
                        for (j = 0; j < SCREEN_WIDTH; ++j)
                        {
                            if(cur_board[_get_index(j, i)])
                            {
                                set_pixel(screenSurface, j, i,
                                    (show_life)?0xFF0000:
                                    (show_heat)?heat_map[_get_index(j, i)]:0);
                            
                                if (heat_map[_get_index(j, i)] < 0xFF)
                                {
                                    heat_map[_get_index(j, i)]++;
                                }
                                else
                                {
                                    if (heat_map[_get_index(j, i)] < 0xFFFF)
                                    {
                                        heat_map[_get_index(j, i)] += 0x0100;
                                    }
                                    else
                                    {
                                        if (heat_map[_get_index(j, i)] != 0xFFFF00)
                                        {
                                            heat_map[_get_index(j, i)] += 0x010000;
                                            heat_map[_get_index(j, i)] -= 0x000001;
                                        }
                                        else
                                        {

                                        }
                                    }
                                }
                                    
                            }
                            else
                            {
                                set_pixel(screenSurface, j, i,((show_heat) ? heat_map[_get_index(j, i)]   : 0));
                            }
                        }
                    
                    }
            
                    /* Swap the boards */
                    swappy = temp_board;
                    temp_board = cur_board;
                    cur_board = swappy;
                }
                
                //Update the surface
                SDL_UpdateWindowSurface( window );

                //Wait two seconds
                SDL_Delay( speed );
                
                while (SDL_PollEvent(&Events))
                {
                    switch (Events.type) {
                        case SDL_QUIT:
                            //Destroy window
                            SDL_DestroyWindow( window );
                            //Quit SDL subsystems
                            SDL_Quit();
                            return 0;
                        break;
                        case SDL_KEYDOWN:
                            if (Events.key.keysym.scancode == SDL_SCANCODE_P) {pause ^= 1;}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_F1){ B[1] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_F2){ B[2] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_F3){ B[3] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_F4){ B[4] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_F5){ B[5] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_F6){ B[6] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_F7){ B[7] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_F8){ B[8] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_F10){ B[0] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_1){ S[1] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_2){ S[2] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_3){ S[3] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_4){ S[4] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_5){ S[5] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_6){ S[6] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_7){ S[7] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_8){ S[8] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_0){ S[0] ^= 1; print_ruleset();}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_H){ show_heat ^= 1;}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_L){ show_life ^= 1;}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_Z){ speed += 5;}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_X){ if(speed > 17){ speed -= 5;}}
                            else if (Events.key.keysym.scancode == SDL_SCANCODE_R)
                            {
                                for (i = 0; i < SCREEN_HEIGHT; ++i)
                                {
                                    for (j = 0; j < SCREEN_WIDTH; ++j)
                                        temp_board[_get_index(j, i)] = get_bool();
                                    for (j = 0; j < SCREEN_WIDTH; ++j)
                                        heat_map[_get_index(j, i)] = 0x0;
                                }
                            }
                        case SDL_KEYUP:
                        break;
                        case SDL_MOUSEBUTTONDOWN:
                            printf("x:%d,y:%d\n", Events.button.x, Events.button.y);
                        case SDL_MOUSEBUTTONUP:
                        break;
                        default:
                        break;
                    }
                }
            }
        }
    }
}
