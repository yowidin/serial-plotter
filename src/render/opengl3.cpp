/**
 * @file   opengl3.cpp
 * @author Dennis Sitelew
 * @date   Sep. 14, 2021
 */

#include <asp/render/opengl3.h>
#include <asp/render/frontend.h>

#include <imgui_impl_opengl3.h>

#include <stdexcept>

using namespace asp::render;

OpenGL3::~OpenGL3() {
   ImGui_ImplOpenGL3_Shutdown();
}

void OpenGL3::init() {
   if (!ImGui_ImplOpenGL3_Init(frontend_->shader_version().c_str())) {
      throw std::runtime_error("ImGui_ImplOpenGL3_Init failed");
   }
}

void OpenGL3::new_frame() {
   ImGui_ImplOpenGL3_NewFrame();
}

void OpenGL3::render() {
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
