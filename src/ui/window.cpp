/**
 * @file   window.cpp
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */

#include <asp/config.h>
#include <asp/options.h>
#include <asp/ui/logs.h>
#include <asp/ui/window.h>

#include <glad/glad.h>

#include <implot.h>

using namespace asp::ui;

namespace {

void throw_sdl_error(const std::string &func) {
   throw std::runtime_error(func + " error: " + SDL_GetError());
}

void throw_errors(const char *context) {
   if (!glad_glGetError) {
      throw std::runtime_error(context);
   }

   std::ostringstream ostream;

   GLenum ec;
   while ((ec = glad_glGetError()) != GL_NO_ERROR) {
      ostream << "OpenGL error inside " << context << ": 0x" << std::hex << ec
              << "\n";
   }

   auto res = ostream.str();
   if (!res.empty()) {
      throw std::runtime_error(res);
   }
}

void glad_callback(const char *name, void *, int, ...) {
   throw_errors(name);
}

void consume_errors(std::string_view context, logs &logs) {
   GLenum ec;
   while ((ec = glad_glGetError()) != GL_NO_ERROR) {
      logs.add("OpenGL error inside ", context, ": 0x", std::hex, ec);
   }
}

} // namespace

asp::po_desc_t window::options::prepare() {
   namespace po = boost::program_options;

   po::options_description od("Window Options");

   // clang-format off
   od.add_options()
       ("full-screen", "Start in full screen")
       ("width", po::value<int>()->default_value(1280), "Window width")
       ("height", po::value<int>()->default_value(900), "Window height")
       ;
   // clang-format on

   return od;
}

window::options window::options::load(po_vars_t &vm) {
   auto full_screen = vm.count("full-screen") != 0;
   auto width = vm["width"].as<int>();
   auto height = vm["height"].as<int>();

   return ui::window::options{full_screen, width, height};
}

window::window(const asp::options &opts,
               logs &logs,
               std::initializer_list<ui::drawable *> drawables)
   : options_{opts.window}
   , drawables_{drawables} {
   // Setup SDL
   if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      throw_sdl_error("SDL_Init");
   }

#if ASP_TARGET_OS(APPLE)
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                       SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                       SDL_GL_CONTEXT_PROFILE_CORE);

   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#elif ASP_TARGET_OS(UNIX)
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif

   auto window_flags =
       (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                         SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);
   if (options_.full_screen) {
      window_flags = (SDL_WindowFlags)(window_flags | SDL_WINDOW_FULLSCREEN);
   }

   window_ = SDL_CreateWindow("Serial Plotter", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, options_.width,
                              options_.height, window_flags);

   context_ = SDL_GL_CreateContext(window_);
   if (!context_) {
      throw_sdl_error("SDL_GL_CreateContext");
   }

   if (SDL_GL_MakeCurrent(window_, context_) != 0) {
      throw_sdl_error("SDL_GL_MakeCurrent");
   }
   SDL_GL_SetSwapInterval(1); // Enable vsync

   // Initialize OpenGL loader
   if (gladLoadGL() == 0) {
      throw_errors("gladLoadGL");
      throw std::runtime_error("gladLoadGL failed");
   }

#ifdef GLAD_DEBUG
   glad_set_post_callback(&glad_callback);
#endif

   consume_errors("setup", logs);

   int profile;
   SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);
   const char *suffix = "";
   if (profile & SDL_GL_CONTEXT_PROFILE_ES) {
      suffix = " ES";
   }

   logs.add("OpenGL version: ", GLVersion.major, ".", GLVersion.minor, suffix);
   std::cout << "OpenGL version: " << GLVersion.major << "." << GLVersion.minor
             << suffix << std::endl;

   frontend_ = std::make_unique<render::frontend>(*window_, context_);

   consume_errors("imgui", logs);
}

window::~window() {
   frontend_.reset();

   if (context_ != nullptr) {
      SDL_GL_DeleteContext(context_);
      context_ = nullptr;
   }

   if (window_ != nullptr) {
      SDL_DestroyWindow(window_);
      window_ = nullptr;
   }
}

void window::update() {
   while (SDL_PollEvent(&event_)) {
      frontend_->process_event(event_);
      if (event_.type == SDL_QUIT) {
         stop_ = true;
      } else if (event_.type == SDL_KEYUP) {
         if (event_.key.keysym.sym == SDLK_AC_BACK ||
             event_.key.keysym.sym == SDLK_ESCAPE) {
            stop_ = true;
         }
      } else if (event_.type == SDL_WINDOWEVENT) {
         switch (event_.window.event) {
            case (SDL_WINDOWEVENT_RESIZED):
               SDL_GetWindowSize(window_, &options_.width, &options_.height);
               break;
         }
      }
   }

   if (ImGui::GetIO().WantTextInput) {
      SDL_StartTextInput();
   } else {
      SDL_StopTextInput();
   }

   frontend_->new_frame();

   auto &io = ImGui::GetIO();
   glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
   glClearColor(clear_color_.x, clear_color_.y, clear_color_.z, clear_color_.w);
   glClear(GL_COLOR_BUFFER_BIT);

   draw();

   frontend_->render();

   SDL_GL_SwapWindow(window_);
}

void window::draw() {
   ImGui::SetNextWindowPos({0, 0});
   ImGui::SetNextWindowSize({static_cast<float>(options_.width),
                             static_cast<float>(options_.height)});

   ImGui::Begin("Main", nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoDecoration |
                    ImGuiWindowFlags_AlwaysAutoResize);

   for (auto &d : drawables_) {
      d->draw();
   }

   ImGui::End();
}
