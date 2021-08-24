/**
 * @file   window.h
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */
#ifndef INCLUDE_ASP_UI_WINDOW_H
#define INCLUDE_ASP_UI_WINDOW_H

#include <asp/options.h>
#include <asp/ui/logs.h>

#include <asp/inputs/data.h>
#include <asp/inputs/serial.h>

#include <SDL2/SDL.h>
#include <imgui.h>
#include <implot.h>
#include <vector>

namespace asp::ui {

class window {
public:
   window(boost::asio::io_context &ctx, const asp::options &opts);
   ~window();

public:
   void start();
   bool can_stop() const { return stop_; }
   void update();

private:
   void draw();

private:
   int width_;
   int height_;
   bool stop_{false};

   SDL_Window *window_{nullptr};
   SDL_Event event_{};
   ImVec4 clear_color_{0.45f, 0.55f, 0.60f, 1.00f};

   logs logs_{"Messages"};
   logs raw_{"RAW"};

   inputs::data data_;
   inputs::serial serial_;
};

} // namespace asp::ui

#endif /* INCLUDE_ASP_UI_WINDOW_H */
