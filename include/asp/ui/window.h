/**
 * @file   window.h
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */
#ifndef INCLUDE_ASP_UI_WINDOW_H
#define INCLUDE_ASP_UI_WINDOW_H

#include <asp/ui/drawable.h>

#include <asp/inputs/data.h>
#include <asp/inputs/serial.h>
#include <asp/render/frontend.h>

#include <SDL2/SDL.h>
#include <imgui.h>
#include <implot.h>
#include <vector>
#include <memory>

namespace asp {

struct options;

namespace ui {

class window {
public:
   struct options {
      bool full_screen{false};
      int width{1280};
      int height{900};

      static po_desc_t prepare();
      static options load(po_vars_t &vm);
   };

public:
   window(const asp::options &opts,
          logs &logs,
          std::initializer_list<ui::drawable *> drawables);
   ~window();

public:
   bool can_stop() const { return stop_; }
   void update();

   void add_drawable(ui::drawable *drawable) {
      drawables_.push_back(drawable);
   }

private:
   void draw();

private:
   asp::ui::window::options options_;

   bool stop_{false};

   SDL_Window *window_{nullptr};
   SDL_GLContext context_;
   SDL_Event event_{};
   std::unique_ptr<render::frontend> frontend_;
   ImVec4 clear_color_{0.45f, 0.55f, 0.60f, 1.00f};

   std::vector<drawable *> drawables_;
};

} // namespace ui
} // namespace asp

#endif /* INCLUDE_ASP_UI_WINDOW_H */
