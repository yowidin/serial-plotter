/**
 * @file   frontend.cpp
 * @author Dennis Sitelew
 * @date   Sep. 14, 2021
 */

#include <asp/config.h>
#include <asp/render/backend.h>
#include <asp/render/frontend.h>

#include <glad/glad.h>

#include <imgui.h>
#include <implot.h>

#include <imgui_impl_sdl2.h>

using namespace asp::render;

frontend::frontend(SDL_Window &window, SDL_GLContext context)
   : window_{&window} {
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGui::StyleColorsDark();
   ImPlot::CreateContext();

   ImGui_ImplSDL2_InitForOpenGL(window_, context);

   const char *glsl_version;
   version v;

   int profile;
   SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);
   bool is_es = profile & SDL_GL_CONTEXT_PROFILE_ES;

   if (GLVersion.major >= 3) {
      if (is_es) {
         v = version::gles3;
         glsl_version = "#version 300 es";
      } else {
         v = version::opengl3;
#if ASP_TARGET_OS(APPLE)
         glsl_version = "#version 150";
#else
         glsl_version = "#version 130";
#endif
      }
   } else {
      if (is_es) {
         v = version::gles2;
      } else {
         v = version::opengl2;
      }
      glsl_version = "#version 100";
   }

   shader_version_ = glsl_version;
   backend_ = backend::make(v, *this);
   backend_->init();
}

frontend::~frontend() {
   backend_.reset();

   ImPlot::DestroyContext();
   ImGui::DestroyContext();
   ImGui_ImplSDL2_Shutdown();
}

void frontend::new_frame() {
   backend_->new_frame();
   ImGui_ImplSDL2_NewFrame(window_);
   ImGui::NewFrame();
}

void frontend::render() {
   ImGui::Render();
   backend_->render();
}

void frontend::process_event(const SDL_Event &event) {
   ImGui_ImplSDL2_ProcessEvent(&event);
}
