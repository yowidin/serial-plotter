/**
 * @file   window.cpp
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */

#include <asp/ui/window.h>

#include <glad/glad.h>

#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <imgui_internal.h>

#include <implot.h>

using namespace asp::ui;

namespace {

void throw_sdl_error(const std::string &func) {
   throw std::runtime_error(func + " error: " + SDL_GetError());
}

} // namespace

window::window(boost::asio::io_context &ctx, const asp::options &opts)
   : width_{opts.width}
   , height_{opts.height}
   , data_{logs_, raw_}
   , serial_{ctx, opts, data_, logs_} {
   // Setup SDL
   if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      throw_sdl_error("SDL_Init");
   }

   // Decide GL+GLSL versions
#if __APPLE__
   // GL 3.2 Core + GLSL 150
   logs_.add_entry("Setting up GL 3.2 + GLS 150");
   const char *glsl_version = "#version 150";
   SDL_GL_SetAttribute(
       SDL_GL_CONTEXT_FLAGS,
       SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                       SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
   // GL 3.0 + GLSL 130
   logs_.add_entry("Setting up GL3.0 + GLS 130");
   const char *glsl_version = "#version 130";
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                       SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

   // Create window with graphics context
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
   SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

   auto window_flags =
       (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
   if (opts.full_screen) {
      window_flags = (SDL_WindowFlags)(window_flags | SDL_WINDOW_FULLSCREEN);
   }

   window_ =
       SDL_CreateWindow("Arduino Serial Plotter", SDL_WINDOWPOS_UNDEFINED,
                        SDL_WINDOWPOS_UNDEFINED, width_, height_, window_flags);

   SDL_GLContext gl_context = SDL_GL_CreateContext(window_);
   if (SDL_GL_MakeCurrent(window_, gl_context) != 0) {
      throw_sdl_error("SDL_GL_MakeCurrent");
   }
   SDL_GL_SetSwapInterval(1); // Enable vsync

   // Initialize OpenGL loader
   if (gladLoadGL() == 0) {
      throw std::runtime_error("gladLoadGL failed");
   }

   logs_.add("OpenGL ", GLVersion.major, "." , GLVersion.minor);
   if (GLVersion.major < 3) {
      throw std::runtime_error(
          "Invalid OpenGL version: " + std::to_string(GLVersion.major) + "." +
              std::to_string(GLVersion.minor));
   }

   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImPlot::CreateContext();

   ImGui_ImplSDL2_InitForOpenGL(window_, gl_context);
   ImGui_ImplOpenGL3_Init();
}

window::~window() {
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplSDL2_Shutdown();
   ImPlot::DestroyContext();
   ImGui::DestroyContext();

   SDL_DestroyWindow(window_);
   SDL_Quit();
}

void window::start() {
   serial_.start();
}

void window::update() {
   while (SDL_PollEvent(&event_)) {
      ImGui_ImplSDL2_ProcessEvent(&event_);
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
               SDL_GetWindowSize(window_, &width_, &height_);
               break;
         }
      }
   }

   if (ImGui::GetIO().WantTextInput) {
      SDL_StartTextInput();
   } else {
      SDL_StopTextInput();
   }

   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplSDL2_NewFrame(window_);
   ImGui::NewFrame();

   draw();

   ImGui::Render();
   auto &io = ImGui::GetIO();
   glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
   glClearColor(clear_color_.x, clear_color_.y, clear_color_.z, clear_color_.w);
   glClear(GL_COLOR_BUFFER_BIT);
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

   SDL_GL_SwapWindow(window_);
}

void window::draw() {
   ImGui::SetNextWindowPos({0, 0});
   ImGui::SetNextWindowSize(
       {static_cast<float>(width_), static_cast<float>(height_)});

   ImGui::Begin("Main", nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoDecoration |
                    ImGuiWindowFlags_AlwaysAutoResize);

   serial_.draw();

   if (ImGui::CollapsingHeader("Logs")) {
      if (ImGui::BeginTabBar("Logs", ImGuiTabBarFlags_None)) {
         if (ImGui::BeginTabItem("RAW")) {
            raw_.draw();
            ImGui::EndTabItem();
         }

         if (ImGui::BeginTabItem("Messages")) {
            logs_.draw();
            ImGui::EndTabItem();
         }
         ImGui::EndTabBar();
      }
   }

   data_.draw();

   ImGui::End();
}
