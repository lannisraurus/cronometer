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
//Constant Definitions
#define WIDTH 720
#define HEIGHT 720
#define VERTICAL_MAX 24
#define HORIZONTAL_MAX 3/100

//Pre-declarations
class line;
SDL_Texture* make_text(std::string text, Uint8 r, Uint8 g, Uint8 b);
TTF_Font* font;
SDL_Renderer* ren;
SDL_Window* win;
SDL_Event ev;

//System variables
bool is_open = true;
bool start_read = false;
int decimal_places = 4;
double uncertainty = 0.5;
int default_lines; //Initial starting point for main text

//Lists of time values
std::vector<int> times;
std::vector<int> time_intervals; //The actual times measured since the start of SDL
double average = 0;

//Lists of text
std::vector<line*> time_gradients; //Lines of text - offset
std::vector<line*> lines; //Lines of text
std::stringstream str_stream; //For rounding

//line class - stores a texture with text on it and a rect
class line{
    private:
        SDL_Texture* texture;
        int size;
        double offset;
    public:
        line(std::string text, double offset, int r, int g, int b){
            this->offset = offset;
            this->size = text.size();
            texture = make_text(text,r,g,b);
        }
        int getSize(){return size;}
        double getOffset(){return offset;}
        SDL_Texture* getTexture(){return texture;}
        ~line(){
            SDL_DestroyTexture(texture);
        }
};

//Make Text Function
SDL_Texture* make_text(std::string text, Uint8 r, Uint8 g, Uint8 b){
    SDL_Surface* title_surf = TTF_RenderText_Solid(font,text.c_str(),{r,g,b});
    SDL_Texture* title_text = SDL_CreateTextureFromSurface(ren,title_surf);
    SDL_FreeSurface(title_surf);
    return title_text;
}

//Display a text list given a reference vertical level 
void display_text_lines(std::vector<line*> array, int vertical_level){
    int iter=0,count=0;
    if(vertical_level>VERTICAL_MAX){iter=vertical_level-VERTICAL_MAX;}
    while(iter < array.size()){
        SDL_Rect line = {(int)(array[iter]->getOffset()*(double)WIDTH),count*HEIGHT/VERTICAL_MAX,array[iter]->getSize()*WIDTH*HORIZONTAL_MAX,HEIGHT/VERTICAL_MAX};
        SDL_RenderCopy(ren,array[iter]->getTexture(),NULL,&line);
        iter++;
        count++;
    }
}

//Record time - start
void time_it(){
    time_intervals.push_back(SDL_GetTicks64());
    int interval = (time_intervals[time_intervals.size()-1] - time_intervals[time_intervals.size()-2]);
    std::string msg = std::to_string(interval)+" ms";
    lines.push_back(new line(msg,0,255,255,255));
    times.push_back(interval);
    if(times.size()>1){
        Uint8 r,g,b=0;
        int gradient = times[times.size()-1]-times[times.size()-2];
        if(gradient > 0){r = 0; g = 255;}
        else if(gradient < 0){ r = 255; g = 0;}
        else{ r = 255; g = 255;}
        std::string msg = std::to_string(gradient)+" ms";
        if(gradient>0){
            msg = "+"+msg;
        }
        time_gradients.push_back(new line(msg,0.7,r,g,b));
    }
    
}

//Clean pointer vector
void clean_pointer_vector(std::vector<line*> &ref){
    for(int i = 0; i < ref.size(); i++){
        delete ref[i];
    }
    while(ref.size()>0){
        ref.erase(ref.begin());
    }
}

//clean everything that needs to be cleaned
void clean_up(){
    clean_pointer_vector(time_gradients);
    clean_pointer_vector(lines);
    time_intervals.clear();
    time_gradients.clear();
    times.clear();
    lines.clear();
}

//Initialize text
void initialize_text(){
    //Initialize menu text - main
    lines.push_back(new line(std::string("Precision - "+std::to_string(decimal_places)+" digits)."),0,255,255,255));
    lines.push_back(new line("SPACE - Start",0,255,255,255));
    lines.push_back(new line("ESC - End",0,255,255,255));
    lines.push_back(new line("---------------------------------",0,255,255,255));
    default_lines = lines.size();
    //Initialize time_gradient text
    for(int i = 0; i < default_lines+2; i++){
        time_gradients.push_back(new line("",0.7,255,255,255));
    }
}

//Start Counting
void start_counting(){
    time_intervals.push_back(SDL_GetTicks64());
    lines.push_back(new line("STARTED",0,255,255,0));
    start_read = true;
}

double get_error(){
    int N = times.size();
    double max_deviation = uncertainty;
    if(N>10){
        //Standard deviation (larger data sets)
        double standard_dev;
        for(int i = 0; i < times.size(); i++){
            standard_dev += pow(average-times[i],2);
        }
        standard_dev/=N;
        standard_dev=sqrt(standard_dev);
        if(standard_dev>max_deviation){max_deviation=standard_dev;}
    }else{
        //For small data sets
        for(int i = 0; i < times.size(); i++){
            double deviation = abs(average-times[i]);
            if(deviation>max_deviation){max_deviation=deviation;}
        }
    }
    return max_deviation;
}

//Stops the count and compile the data
void stop_counting(){
    average = 0;
    for(int i = 0; i < times.size(); i++){
        average+=times[i];
    }
    average/=times.size();
    str_stream.clear();
    str_stream << std::setprecision(decimal_places) << average;
    std::string iter;
    str_stream >> iter;

    str_stream.clear();
    str_stream << std::setprecision(decimal_places) << get_error();
    std::string error;
    str_stream >> error;

    std::string msg = "Avg: "+iter+" +/- "+error+" ms";
    lines.push_back(new line(msg,0,255,255,0));
    start_read = false;
}

//Handle window events and input
void handle_events(){
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
                            time_it();
                        }else{
                            clean_up();
                            initialize_text();
                            start_counting();
                        }
                        break;
                    case SDLK_ESCAPE:
                        if(start_read==true){
                            stop_counting();
                        }
                        break;
                }
            }
        }
}

//MAIN
int main(){
    //Initialize SDL and TTF
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    //SDL Variables - Initialize
    win = SDL_CreateWindow("Cronometer - Periodic Intervals",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,SDL_WINDOW_SHOWN);
    ren = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED);
    //Text LOAD
    font = TTF_OpenFont("slkscr.ttf",22);
    //Initialize basic menu text
    initialize_text();
    //App Loop
    while(is_open){
        //Events
        handle_events();

        //Clear Renderer
        SDL_SetRenderDrawColor(ren,0,0,0,255);
        SDL_RenderClear(ren);

        //Render Text lines
        display_text_lines(lines,lines.size());
        display_text_lines(time_gradients,lines.size());
        
        //Show
        SDL_RenderPresent(ren);
    }
    //Clean up
    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(ren);
    SDL_Quit();
}