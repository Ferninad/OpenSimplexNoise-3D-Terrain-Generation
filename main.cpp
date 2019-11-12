#include "common.h"
#include "cmath"
#include "OpenSimplexNoise.h"

bool Init();
void CleanUp();
void Run();
void Noise();
void Rots();
void Draw();
void Connect(int i, int j, vector<vector<double>> pps);
vector<vector<double>> MultMatrixs(vector<vector<double>> mat1, vector<vector<double>> mat2);
double ScaleNum(double n, double minN, double maxN, double min, double max);

SDL_Window *window;
SDL_GLContext glContext;
SDL_Surface *gScreenSurface = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Rect pos;

double screenWidth = 501;
double screenHeight = 501;
double resolution = 50;
double featureSize = 200;
double yang = 0;
double xang = 1.2208;
//double xang = 3.14159265 / 2;
double zang = 0;
int cornerSize = 1;
int lineSize = 1;
double xper = 1;
double yper = 1;
int offset = 0;
int amp = 100;
bool change = true;
OpenSimplexNoise *noise1 = nullptr;
const double PI = 3.14159265;

vector<vector<double>> points;
vector<vector<double>> rotx;
vector<vector<double>> roty;
vector<vector<double>> rotz;
vector<vector<double>> projection;

bool Init()
{
    if (SDL_Init(SDL_INIT_NOPARACHUTE & SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialised");
    }

    //Create Window Instance
    window = SDL_CreateWindow(
        "Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,   
        SDL_WINDOW_OPENGL);

    //Check that the window was succesfully created
    if (window == NULL)
    {
        //Print error, if null
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }
    else{
        gScreenSurface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Log("Window Successful Generated");
    }
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);

    return true;
}

int main()
{
    //Error Checking/Initialisation
    if (!Init())
    {
        printf("Failed to Initialize");
        return -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Swap Render Buffers
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Run();

    CleanUp();
    return 0;
}

void CleanUp()
{
    //Free up resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Run()
{
    bool gameLoop = true;
    srand(time(NULL));
    long rand1 = rand() * (RAND_MAX + 1) + rand();
    noise1 = new OpenSimplexNoise{rand1};

    Rots();
    while (gameLoop)
    {   
        //if(change){
            offset-=4;
            change = false;
            pos.x = 0;
            pos.y = 0;
            pos.w = screenWidth;
            pos.h = screenHeight;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &pos);
            projection.clear();
            projection.push_back({xper, 0, 0});
            projection.push_back({0, yper, 0});
            Noise();
            Draw();
            SDL_RenderPresent(renderer);
        //}
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameLoop = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    gameLoop = false;
                    break;
                case SDLK_w:
                    featureSize++;
                    change = true;
                    break;
                case SDLK_s:
                    featureSize--;
                    change = true;
                    break;
                case SDLK_q:
                    xper += .1;
                    change = true;
                    break;
                case SDLK_a:
                    xper -= .1;
                    change = true;
                    if(xper < 0)
                        xper = 0;
                    break;
                case SDLK_e:
                    yper += .1;
                    change = true;
                    break;
                case SDLK_d:
                    yper -= .1;
                    change = true;
                    if(yper < 0)
                        yper = 0;
                    break;
                case SDLK_SPACE:
                    rand1 = rand() * (RAND_MAX + 1) + rand();
                    noise1 = new OpenSimplexNoise{rand1};
                    change = true;
                case SDLK_UP:
                    xang += .01;
                    change = true;
                    break;
                case SDLK_DOWN:
                    xang -= .01;
                    change = true;
                    if(xang < 0)
                        xang = 0;
                    break;
                default:
                    break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                default:
                    break;
                }
            }
        }
    }
}

void Noise(){
    points.clear();
    for(double x = ceil(-1 * screenWidth / 2); x < ceil(screenWidth / 2) ; x+=resolution){
        for(double y = ceil(-1 * screenHeight / 2) + offset; y < ceil(screenHeight / 2) + offset; y+=resolution){
            double noise = (*noise1).eval(x/featureSize, y/featureSize);
            noise = ScaleNum(noise, -1, 1, 0, amp);
            
            vector<double> temp;
            temp = {x, y - offset, noise};
            points.push_back(temp);
        }
    }
}

void Draw(){
    vector<vector<double>> pps;
    vector<vector<double>> xyz;
    vector<double> temp;
    temp.clear();
    Rots();
    for(int i = 0; i < points.size(); i++){
        xyz.clear();
        temp.clear();
        for(int j = 0; j < points[i].size(); j++){
            temp.push_back(points[i][j]);
            xyz.push_back(temp);
            temp.clear();
        }
        vector<vector<double>> rotated = MultMatrixs(roty, xyz);
        rotated = MultMatrixs(rotx, rotated);
        rotated = MultMatrixs(rotz, rotated);
        vector<vector<double>> xys = MultMatrixs(projection, rotated);
        for(int j = 0; j < xys.size(); j++){
            temp.push_back(xys[j][0]);
        }
        pps.push_back(temp);

        pos.x = pps[i][0] + ceil(screenWidth / 2);
        pos.y = pps[i][1] + ceil(screenHeight / 2);
        pos.w = cornerSize;
        pos.h = cornerSize;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &pos);
    }
    vector<vector<double>> grid;
    temp.clear();
    for(int i = 0; i < pps.size(); i+=screenHeight/resolution + 1){
        for(int j = i; j < round(i + screenHeight/resolution); j++){
            Connect(j, j + 1, pps);
        }
    }
    for(int i = 0; i < round(screenHeight/resolution + 1); i++){
        for(int j = i; j < pps.size() - round(screenHeight/resolution + 1); j+=screenHeight/resolution + 1){
            Connect(j, j + screenHeight/resolution + 1, pps);
        }
    }
    for(int i = 1; i < round(screenHeight/resolution + 1); i++){
        for(int j = i; j < pps.size() - round(screenHeight/resolution + 1); j+=screenHeight/resolution + 1){
            Connect(j, j + screenHeight/resolution + 1 - 1, pps);
        }
    }
}

void Connect(int i, int j, vector<vector<double>> pps){
    int ix = pps[i][0] + floor(screenWidth / 2);
    int iy = pps[i][1] + floor(screenHeight / 2);
    int jx = pps[j][0] + floor(screenWidth / 2);
    int jy = pps[j][1] + floor(screenHeight / 2);
    double slope = static_cast<double>((jy - iy)) / (jx - ix);
    double cang = atan2((jy - iy), (jx - ix));
    double slopex = cos(cang);
    if(jx > ix && abs(slope) < 40){
        for(double x = jx; x >= ix; x-=abs(slopex)){
            pos.x = x + (cornerSize - lineSize) / 2;
            pos.y = slope * x + slope * (-1 * jx) + jy + (cornerSize - lineSize) / 2;
            pos.w = lineSize;
            pos.h = lineSize;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &pos);
        }
    }
    else if(jx < ix && abs(slope) < 40){
        for(double x = jx; x <= ix; x+=abs(slopex)){
            pos.x = x + (cornerSize - lineSize) / 2;
            pos.y = slope * x + slope * (-1 * jx) + jy + (cornerSize - lineSize) / 2;
            pos.w = lineSize;
            pos.h = lineSize;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &pos);
        }
    }
    else{
        if(jy < iy){
            for(double y = jy; y != iy; y+=1){
                pos.x = jx + (cornerSize - lineSize) / 2;
                pos.y = y + (cornerSize - lineSize) / 2;
                pos.w = lineSize;
                pos.h = lineSize;
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &pos);
            }
        }
        else if(jy > iy){
            for(double y = jy; y != iy; y-=1){
                pos.x = jx + (cornerSize - lineSize) / 2;
                pos.y = y + (cornerSize - lineSize) / 2;
                pos.w = lineSize;
                pos.h = lineSize;
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &pos);
            }
        }
    }
}

vector<vector<double>> MultMatrixs(vector<vector<double>> mat1, vector<vector<double>> mat2){
    vector<vector<double>> result;
    vector<double> temp;
    double a = 0;
    for(int j = 0; j < mat1.size(); j++){
        for(int k = 0; k < mat2[0].size(); k++){
            for(int i = 0; i < mat1[j].size(); i++){
                a+= mat1[j][i] * mat2[i][k];
            }
            temp.push_back(a);
            a = 0;
        }
        result.push_back(temp);
        temp.clear();
    }
    return result;
}

void Rots(){
    vector<double> temp;
    rotx.clear();
    roty.clear();
    rotz.clear();

    temp.clear();
    temp.push_back(1);
    temp.push_back(0);
    temp.push_back(0);
    rotx.push_back(temp);
    temp.clear();
    temp.push_back(0);
    temp.push_back(cos(xang));
    temp.push_back(-1*sin(xang));
    rotx.push_back(temp);
    temp.clear();
    temp.push_back(0);
    temp.push_back(sin(xang));
    temp.push_back(cos(xang));
    rotx.push_back(temp);
    temp.clear();

    temp.push_back(cos(yang));
    temp.push_back(0);
    temp.push_back(sin(yang));
    roty.push_back(temp);
    temp.clear();
    temp.push_back(0);
    temp.push_back(1);
    temp.push_back(0);
    roty.push_back(temp);
    temp.clear();
    temp.push_back(-1*sin(yang));
    temp.push_back(0);
    temp.push_back(cos(yang));
    roty.push_back(temp);
    temp.clear();

    temp.push_back(cos(zang));
    temp.push_back(-1*sin(zang));
    temp.push_back(0);
    rotz.push_back(temp);
    temp.clear();
    temp.push_back(sin(zang));
    temp.push_back(cos(zang));
    temp.push_back(0);
    rotz.push_back(temp);
    temp.clear();
    temp.push_back(0);
    temp.push_back(0);
    temp.push_back(1);
    rotz.push_back(temp);
    temp.clear();
}

double ScaleNum(double n, double minN, double maxN, double min, double max){
    return (((n - minN) / (maxN - minN)) * (max - min)) + min;
}