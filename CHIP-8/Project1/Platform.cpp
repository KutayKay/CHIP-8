#include "Platform.h"
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




class Platform
{
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* texture;
		Uint32* pixels;
	public:
		Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
		{
			SDL_Init(SDL_INIT_VIDEO);

			window = SDL_CreateWindow(title, windowWidth, windowHeight, SDL_WINDOW_HIDDEN);
			renderer = SDL_CreateRenderer(window, "Renderer1");
			texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, textureWidth, textureHeight);
			pixels = new Uint32[textureWidth * textureHeight];

			memset(pixels, 255, textureWidth * textureHeight * sizeof(Uint32));
		}

		~Platform()
		{
			SDL_DestroyTexture(texture);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}

		void Update(void const* buffer, int pitch)
		{
			SDL_UpdateTexture(texture, nullptr, buffer, pitch);
			SDL_RenderClear(renderer);
			SDL_RenderTexture(renderer, texture, nullptr, nullptr);
			SDL_RenderPresent(renderer);
		}

		bool ProcessInput(uint8_t* keys)
		{
			bool quit = false;

			SDL_Event event;

			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{

				case SDL_EVENT_QUIT:
				{
					quit = true;
				} break;

				case SDL_EVENT_KEY_DOWN:
				{
					switch (event.key.key)
					{
						case SDLK_ESCAPE:
						{
							quit = true;
						}
						case SDLK_X:
						{
							keys[0] = 1;
						} break;
						case SDLK_1:
						{
							keys[1] = 1;
						} break;
						case SDLK_2:
						{
							keys[2] = 1;
						} break;
						case SDLK_3:
						{
							keys[3] = 1;
						} break;
						case SDLK_Q:
						{
							keys[4] = 1;
						} break;
						case SDLK_W:
						{
							keys[5] = 1;
						} break;
						case SDLK_E:
						{
							keys[6] = 1;
						} break;
						case SDLK_A:
						{
							keys[7] = 1;
						} break;
						case SDLK_S:
						{
							keys[8] = 1;
						} break;
						case SDLK_D:
						{
							keys[9] = 1;
						} break;
						case SDLK_Z:
						{
							keys[0xA] = 1;
						} break;
						case SDLK_C:
						{
							keys[0xB] = 1;
						} break;
						case SDLK_4:
						{
							keys[0xC] = 1;
						} break;
						case SDLK_R:
						{
							keys[0xD] = 1;
						} break;
						case SDLK_F:
						{
							keys[0xE] = 1;
						} break;
						case SDLK_V:
						{
							keys[0xF] = 1;
						} break;
					}
				} break;

				case SDL_EVENT_KEY_UP:
				{
					switch (event.key.key)
					{
						case SDLK_X:
						{
							keys[0] = 1;
						} break;
						case SDLK_1:
						{
							keys[1] = 1;
						} break;
						case SDLK_2:
						{
							keys[2] = 1;
						} break;
						case SDLK_3:
						{
							keys[3] = 1;
						} break;
						case SDLK_Q:
						{
							keys[4] = 1;
						} break;
						case SDLK_W:
						{
							keys[5] = 1;
						} break;
						case SDLK_E:
						{
							keys[6] = 1;
						} break;
						case SDLK_A:
						{
							keys[7] = 1;
						} break;
						case SDLK_S:
						{
							keys[8] = 1;
						} break;
						case SDLK_D:
						{
							keys[9] = 1;
						} break;
						case SDLK_Z:
						{
							keys[0xA] = 1;
						} break;
						case SDLK_C:
						{
							keys[0xB] = 1;
						} break;
						case SDLK_4:
						{
							keys[0xC] = 1;
						} break;
						case SDLK_R:
						{
							keys[0xD] = 1;
						} break;
						case SDLK_F:
						{
							keys[0xE] = 1;
						} break;
						case SDLK_V:
						{
							keys[0xF] = 1;
						} break;
					}
				} break;
			}
		}
		return quit;
	}
};