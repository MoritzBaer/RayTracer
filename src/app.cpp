#include "Image.h"
#include "SDL.h"
#include <cstdint>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define A_VALUE(val) (val)
#define R_VALUE(val) ((val) << 8)
#define G_VALUE(val) ((val) << 16)
#define B_VALUE(val) ((val) << 24)
#else
#define A_VALUE(val) ((val) << 24)
#define R_VALUE(val) ((val) << 16)
#define G_VALUE(val) ((val) << 8)
#define B_VALUE(val) (val)
#endif


class DisplayApp {
public:
  DisplayApp()
      : isRunning(false), window(nullptr), renderer(nullptr),
        displayTexture(nullptr), displayImage() {};
  ~DisplayApp() {};

  bool Initialize();
  void Run();
  void Cleanup();

private:
  bool isRunning;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *displayTexture;
  ImageT<480, 9 * 480 / 16, uint32_t> displayImage;
};

#undef main

int main() {
  DisplayApp app{};
  if (!app.Initialize()) {
    return -1;
  }
  app.Run();
  app.Cleanup();
  return 0;
}

bool DisplayApp::Initialize() {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    return false;
  }

  window = SDL_CreateWindow("TraceViewer", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, displayImage.W, displayImage.H, 0);
  if (!window) {
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    return false;
  }

  uint32_t rMask = R_VALUE(0xff);
  uint32_t gMask = G_VALUE(0xff);
  uint32_t bMask = B_VALUE(0xff);
  uint32_t aMask = A_VALUE(0xff);

  if (displayTexture) {
    SDL_DestroyTexture(displayTexture);
  }

  auto surf = SDL_CreateRGBSurface(0, displayImage.W, displayImage.H, 32, rMask,
                                   gMask, bMask, aMask);
  displayTexture = SDL_CreateTextureFromSurface(renderer, surf);
  SDL_FreeSurface(surf);

  return true;
}

void DisplayApp::Run() {
  SDL_Event currentEvent;

  isRunning = true;
  while (isRunning) {
    while (SDL_PollEvent(&currentEvent)) {
      if (currentEvent.type == SDL_QUIT) {
        isRunning = false;
      }
    }

    // Draw Background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Draw image
    // TODO: Only do when change happens
    displayImage.UploadToTexture(displayTexture);
    for (int x = 0; x < displayImage.W; x++) {
        for (int y = 0; y < displayImage.H; y++) {
            auto r = uint32_t((255 * x) / displayImage.W);
            auto g = uint32_t((255 * y) / displayImage.H);
            displayImage.at(x, y) = A_VALUE(0xff) + R_VALUE(r) + G_VALUE(g);
        }
    }
    SDL_Rect bounds = {
        .x = 0, .y = 0, .w = displayImage.W, .h = displayImage.H};

    SDL_RenderCopy(renderer, displayTexture, &bounds, &bounds);

    // Present to window
    SDL_RenderPresent(renderer);
  }
}

void DisplayApp::Cleanup() {
  if (renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }
  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }
  if (displayTexture) {
    SDL_DestroyTexture(displayTexture);
    displayTexture = nullptr;
  }
  SDL_Quit();
}
