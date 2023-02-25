//Regular
#include <iostream>
//Graphics
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
//Math
#include <chrono>
#include <math.h>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
//Constants
#define WIDTH 720
#define HEIGHT 720


int main(){
    //Initialize
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    //SDL Variables
    SDL_Window* win = SDL_CreateWindow("Cronometer - Periodic Intervals",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED);
    SDL_Event ev;
    //System variables
    bool is_open = true;
    bool start_read = false;
    int decimal_places = 3;
    double uncertainty = pow(10,-decimal_places)/2;
    double *intervals;
    std::stringstream str_stream;
    //Textures - Loading
    TTF_Font* font = TTF_OpenFont("\\munro.ttf",32);
    if(font==nullptr){std::cerr<<"Error: couldn't load font\n";}
    std::string title_msg = "Precision is "+std::to_string(decimal_places)+" decimal places.\nSPACE - Start\nESC - End";
    SDL_Surface* title_surf = TTF_RenderText_Solid(font,"test",{255,255,255});
    SDL_Texture* title_text = SDL_CreateTextureFromSurface(ren,title_surf);
    SDL_FreeSurface(title_surf);
    SDL_Rect title_rect = {0,0,WIDTH,HEIGHT/6};
    //App Loop
    while(is_open){
        //Events
        while(SDL_PollEvent(&ev)){
            switch(ev.type){
                default: break;
                case SDL_QUIT:
                    is_open = false;
                    break;
                case SDL_KEYDOWN:
                if(start_read==true){
                    switch(ev.key.keysym.sym){
                        default: break;
                        case SDLK_SPACE:
                            
                            break;
                        case SDLK_ESCAPE:

                            start_read = false;
                            break;
                    }
                }else{
                    //Insert clean screen code here
                    start_read = true;
                }
            }
        }


        SDL_SetRenderDrawColor(ren,0,0,0,255);
        SDL_RenderClear(ren);

        SDL_RenderCopy(ren,title_text,NULL,&title_rect);

        SDL_RenderPresent(ren);

    }
    //Clean up
    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(ren);
    SDL_Quit();

}