#pragma once

#include "SDL.h"
#include <array>
#include <concepts>
#include <cstdint>

template <uint16_t w, uint16_t h, typename Value_T> struct ImageT {
private:
  std::array<Value_T, w * h> pixels;

public:
  inline static constexpr uint16_t W = w;
  inline static constexpr uint16_t H = h;
  ImageT() : pixels() {}

  inline constexpr Value_T &at(uint16_t const &x, uint16_t const &y) {
    return pixels[x + w * y];
  }
  inline constexpr Value_T const &at(uint16_t const &x,
                                     uint16_t const &y) const {
    return pixels[x + w * y];
  }

  inline Value_T *getPixels() { return &pixels; }

  inline void UploadToTexture(SDL_Texture *target) {
    SDL_UpdateTexture(target, nullptr, &pixels, w * sizeof(Value_T));
  }
};