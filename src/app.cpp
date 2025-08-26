#include "Image.h"
#include "RayTracingManager.h"
#include "SDL.h"
#include <cstdint>
#include "IntegerPacking.h"
#include "RGBTracer.h"

// TODO: Write concept for ContentCreator
template <class ContentCreator>
class DisplayApp
{
    bool isRunning;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *displayTexture;
    ImageT<uint32_t> displayImage;
    std::mutex displayMutex;
    ContentCreator creator;

public:
    DisplayApp()
        : isRunning(false), window(nullptr), renderer(nullptr),
          displayTexture(nullptr), displayImage(1600, 900), creator(displayImage, displayMutex, isRunning), displayMutex() {};
    ~DisplayApp() {};

    bool Initialize();
    template <typename... CreatorArgs>
    void Run(CreatorArgs... args);
    void Cleanup();
};

#undef main

int main()
{
    DisplayApp<RayTracingManager<RGBTracer>> app{};
    if (!app.Initialize())
    {
        return -1;
    }
    app.Run(Scene{});
    app.Cleanup();
    return 0;
}

template <class ContentCreator>
bool DisplayApp<ContentCreator>::Initialize()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        return false;
    }

    window = SDL_CreateWindow("TraceViewer", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, displayImage.W, displayImage.H, 0);
    if (!window)
    {
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        return false;
    }

    uint32_t rMask = R_VALUE(0xff);
    uint32_t gMask = G_VALUE(0xff);
    uint32_t bMask = B_VALUE(0xff);
    uint32_t aMask = A_VALUE(0xff);

    if (displayTexture)
    {
        SDL_DestroyTexture(displayTexture);
    }

    auto surf = SDL_CreateRGBSurface(0, displayImage.W, displayImage.H, 32, rMask,
                                     gMask, bMask, aMask);
    displayTexture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    return true;
}

template <class ContentCreator>
template <typename... CreatorArgs>
void DisplayApp<ContentCreator>::Run(CreatorArgs... args)
{
    SDL_Event currentEvent;

    std::thread generationThread([this](CreatorArgs... args)
                                 { this->creator.GenerateContent(args...); }, args...);

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

        // Draw Background
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Draw image
        if (displayMutex.try_lock())
        {
            displayImage.UploadToTexture(displayTexture);
            displayMutex.unlock();

            SDL_Rect bounds = {
                .x = 0, .y = 0, .w = displayImage.W, .h = displayImage.H};

            SDL_RenderCopy(renderer, displayTexture, &bounds, &bounds);

            // Present to window
            SDL_RenderPresent(renderer);
        }
    }

    generationThread.join();
}

template <class ContentCreator>
void DisplayApp<ContentCreator>::Cleanup()
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
    if (displayTexture)
    {
        SDL_DestroyTexture(displayTexture);
        displayTexture = nullptr;
    }
    SDL_Quit();
}
