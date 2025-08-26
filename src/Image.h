#pragma once

#include "SDL.h"
#include <algorithm>
#include <concepts>
#include <cstdint>
#include <memory>

template <typename T_Conv, typename T1, typename T2>
concept Converter = requires(T_Conv const &converter, T1 const &value) {
  { converter(value) } -> std::same_as<T2>;
};

template <typename Value_T>
struct ImageT
{
private:
  Value_T *const pixels;

public:
  uint16_t const W;
  uint16_t const H;
  ImageT(uint16_t w, uint16_t h) : pixels(new Value_T[w * h]), W(w), H(h) {
    std::fill(pixels, pixels + W * H, Value_T());
  }
  ImageT(ImageT const &other) : W(other.W), H(other.H), pixels(new Value_T[other.W * other.H]) {
    memcpy(pixels, other.pixels, W * H * sizeof(Value_T));
  }
  ImageT(ImageT &&other) noexcept : W(other.W), H(other.H), pixels(other.pixels)
  {
    other.pixels = nullptr;
  };
  ImageT() : W(0), H(0), pixels(nullptr) {}
  ~ImageT()
  {
    delete[] pixels;
  }

  ImageT &operator=(ImageT const &other)
  {
    if (this == &other)
      return *this;

    std::destroy_at(this);
    std::construct_at(this, std::move(other));

    return *this;
  }

  inline constexpr Value_T &at(uint16_t const &x, uint16_t const &y)
  {
    return pixels[x + W * y];
  }
  inline constexpr Value_T const &at(uint16_t const &x,
                                     uint16_t const &y) const
  {
    return pixels[x + W * y];
  }

  inline Value_T *getPixels() { return &pixels; }

  inline void UploadToTexture(SDL_Texture *target)
  {
    SDL_UpdateTexture(target, nullptr, pixels, W * sizeof(Value_T));
  }

  template <typename otherValue_T, Converter<otherValue_T, Value_T> Converter>
  inline void Set(ImageT<otherValue_T> const &other, uint16_t xOffset, uint16_t yOffset, Converter const &converter)
  {
    for (uint16_t x = 0; x < other.W; x++)
    {
      for (uint16_t y = 0; y < other.H; y++)
      {
        this->at(x + xOffset, y + yOffset) = converter(other.at(x, y));
      }
    }
  }
};