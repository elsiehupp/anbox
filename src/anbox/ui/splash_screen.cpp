/*
 * Copyright (C) 2017 Simon Fels <morphis@gravedo.de>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "anbox/ui/splash_screen.h"
#include "anbox/utils.h"

#include <SDL2/SDL_image.h>

namespace anbox {
namespace ui {
SplashScreen::SplashScreen() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
    const auto message = utils::string_format("Failed to initialize SDL: %s", SDL_GetError());
    BOOST_THROW_EXCEPTION(std::runtime_error(message));
  }

  const auto width = 1024, height = 768;
  window_ = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                             width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
  if (!window_) {
    const auto message = utils::string_format("Failed to create window: %s", SDL_GetError());
    BOOST_THROW_EXCEPTION(std::runtime_error(message));
  }

  auto surface = SDL_GetWindowSurface(window_);
  SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 0xee, 0xee, 0xee));
  SDL_UpdateWindowSurface(window_);

  auto renderer = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  auto img = IMG_LoadTexture(renderer, "/snap/anbox/current/snap/gui/icon.png");

  const auto tex_width = 128, tex_height = 128;
  SDL_Rect r{(width - tex_width) / 2, (height - tex_height) / 2, 128, 128};

  SDL_SetRenderDrawColor(renderer, 0xee, 0xee, 0xee, 0xff);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, img, nullptr, &r);
  SDL_RenderPresent(renderer);

  SDL_ShowWindow(window_);

  event_thread_ = std::thread(&SplashScreen::process_events, this);
}

SplashScreen::~SplashScreen() {
  if (event_thread_running_) {
    event_thread_running_ = false;
    if (event_thread_.joinable())
      event_thread_.join();
  }

  if (window_)
    SDL_DestroyWindow(window_);
}

void SplashScreen::process_events() {
  event_thread_running_ = true;
  while (event_thread_running_) {
    SDL_Event event;
    while (SDL_WaitEventTimeout(&event, 100)) {
      switch (event.type) {
      case SDL_QUIT:
        break;
      default:
        break;
      }
    }
  }
}
} // namespace ui
} // namespace anbox
