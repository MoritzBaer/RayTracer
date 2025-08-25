#pragma once

#include "SDL.h"

class DisplayApp
{
public:
    DisplayApp() : isRunning(false), window(nullptr), renderer(nullptr) {};
    ~DisplayApp() {};

    bool Initialize();
    void Run();
    void Cleanup();

private:
    bool isRunning;
    SDL_Window *window;
    SDL_Renderer *renderer;
};

#undef main

int main()
{
    DisplayApp app{};
    if (!app.Initialize())
    {
        return -1;
    }
    app.Run();
    app.Cleanup();
    return 0;
}

bool DisplayApp::Initialize()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        return false;
    }

    window = SDL_CreateWindow("TraceViewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, 0);
    if (!window)
    {
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        return false;
    }

    return true;
}

void DisplayApp::Run()
{
    SDL_Event currentEvent;

    isRunning = true;
    while (isRunning)
    {
        while (SDL_PollEvent(&currentEvent))
        {
            if (currentEvent.type == SDL_QUIT)
            {
                isRunning = false;
            }
        }

        SDL_RenderClear(renderer);
        // Render your scene here
        SDL_RenderPresent(renderer);
    }
}

void DisplayApp::Cleanup()
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}
