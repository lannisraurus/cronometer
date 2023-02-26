//Regular
#include <iostream>
//Graphics
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
//Math
#include <math.h>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
//Constants
#define WIDTH 720
#define HEIGHT 720

//Initialize SDL Pointers
TTF_Font* font;
SDL_Renderer* ren;
SDL_Window* win;
SDL_Event ev;

//System variables
bool is_open = true;
bool start_read = false;
int decimal_places = 4;
double uncertainty = pow(10,-decimal_places)/2;
std::vector<double> time_intervals;
std::vector<double> times;
std::stringstream str_stream;

//Make Text Function
SDL_Texture* make_text(std::string text){
    SDL_Surface* title_surf = TTF_RenderText_Solid(font,text.c_str(),{255,255,255});
    SDL_Texture* title_text = SDL_CreateTextureFromSurface(ren,title_surf);
    SDL_FreeSurface(title_surf);
    return title_text;
}

//line class - stores a texture and a rect
class line{
    private:
        SDL_Texture* texture;
        int size;
        double offset;
    public:
        line(std::string text, double offset){
            this->offset = offset;
            this->size = text.size();
            texture = make_text(text);
        }
        int getSize(){return size;}
        double getOffset(){return offset;}
        SDL_Texture* getTexture(){return texture;}
        ~line(){
            SDL_DestroyTexture(texture);
        }
};

//MAIN
int main(){
    //Initialize SDL
    std::cout << "Starting\n";
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    //SDL Variables - Initialize
    win = SDL_CreateWindow("Cronometer - Periodic Intervals",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,SDL_WINDOW_SHOWN);
    ren = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED);
    //Text
    font = TTF_OpenFont("slkscr.ttf",22);
    if(font==nullptr){std::cerr<<"Error: couldn't load font\n";}

    std::string title_msg = "Precision - "+std::to_string(decimal_places)+" digits.";
    
    std::vector<line*> lines;
    lines.push_back(new line(title_msg,0));
    lines.push_back(new line("SPACE - Start",0));
    lines.push_back(new line("ESC - End",0));
    lines.push_back(new line("---------------------------------",0));
    int default_lines = lines.size();

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
                switch(ev.key.keysym.sym){
                    case SDLK_SPACE:
                        if(start_read==true){
                            str_stream.clear();
                            time_intervals.push_back(SDL_GetTicks64());
                            double interval = (time_intervals[time_intervals.size()-1] - time_intervals[time_intervals.size()-2]);
                            str_stream << std::setprecision(decimal_places) << interval;
                            std::string iter;
                            str_stream >> iter;
                            iter+=" ms";
                            lines.push_back(new line(iter,0));
                            times.push_back(interval);
                        }else{
                            time_intervals.clear();
                            times.clear();
                            lines.erase(lines.begin()+default_lines,lines.begin()+lines.size());
                            time_intervals.push_back(SDL_GetTicks64());
                            lines.push_back(new line("STARTED",0));
                            start_read = true;
                        }
                        break;
                    case SDLK_ESCAPE:
                        if(start_read==true){
                            double average = 0;
                            for(int i = 0; i < times.size(); i++){
                                average+=times[i];
                            }
                            average/=times.size();
                            str_stream.clear();
                            str_stream << std::setprecision(decimal_places) << average;
                            std::string iter;
                            str_stream >> iter;
                            std::string msg = "Average: "+iter;
                            lines.push_back(new line(msg,0));
                            start_read = false;
                        }
                        break;
                }
            }
        }

        //Clear Renderer
        SDL_SetRenderDrawColor(ren,0,0,0,255);
        SDL_RenderClear(ren);

        //Render Text lines
        for(int i = 0; i < lines.size();i++){
            SDL_Rect line = {lines[i]->getOffset()*WIDTH,i*WIDTH/24,lines[i]->getSize()*3*WIDTH/100,HEIGHT/24};
            SDL_RenderCopy(ren,lines[i]->getTexture(),NULL,&line);
        }
        
        //Show
        SDL_RenderPresent(ren);
    }
    //Clean up
    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(ren);
    SDL_Quit();

}