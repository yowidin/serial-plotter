/**
 * @file   gles2.cpp
 * @author Dennis Sitelew
 * @date   Sep. 14, 2021
 */

#include <asp/render/gles2.h>
#include <asp/render/frontend.h>

#include <asp/render/detail/imgui_impl_sdl_es2.h>

#include <stdexcept>

using namespace asp::render;

GLES2::~GLES2() {
   ImGui_ImplSdlGLES2_Shutdown();
}

void GLES2::init() {
   if (!ImGui_ImplSdlGLES2_Init()) {
      throw std::runtime_error("ImGui_ImplSdlGLES2_Init failed");
   }
}

void GLES2::new_frame() {
   ImGui_ImplSdlGLES2_NewFrame(&frontend_->window());
}

void GLES2::render() {
   ImGui_ImplSdlGLES2_RenderDrawLists(ImGui::GetDrawData());
}
