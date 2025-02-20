#pragma once

#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDl3/SDL_render.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_system.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_surface.h>
#include <random>



class Platform
{
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        Uint32* pixels;

    public:
        Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
        ~Platform();

        void Update(void const* buffer, int pitch);
        bool ProcessInput(uint8_t* keys);
};

#endif // PLATFORM_H
