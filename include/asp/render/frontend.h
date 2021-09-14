/**
 * @file   frontend.h
 * @author Dennis Sitelew
 * @date   Sep. 14, 2021
 */
#ifndef INCLUDE_ASP_RENDER_FRONTEND_H
#define INCLUDE_ASP_RENDER_FRONTEND_H

#include <SDL2/SDL.h>

#include <memory>
#include <string>

namespace asp::render {

class backend;

class frontend {
public:
   frontend(SDL_Window &window, SDL_GLContext context);
   ~frontend();

public:
   void new_frame();
   void render();
   void process_event(const SDL_Event &event);

   const std::string &shader_version() const { return shader_version_; }

   SDL_Window &window() { return *window_; }

private:
   SDL_Window *window_;
   std::unique_ptr<backend> backend_;
   std::string shader_version_;
};

} // namespace asp::render

#endif /* INCLUDE_ASP_RENDER_FRONTEND_H */
