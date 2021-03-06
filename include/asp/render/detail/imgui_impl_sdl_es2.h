// ImGui SDL2 binding with OpenGL2
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#ifndef IMGUI_IMPL_SDL_ES2
#define IMGUI_IMPL_SDL_ES2

#include <imgui.h>

struct SDL_Window;
typedef union SDL_Event SDL_Event;

IMGUI_API bool        ImGui_ImplSdlGLES2_Init();
IMGUI_API void        ImGui_ImplSdlGLES2_Shutdown();
IMGUI_API void        ImGui_ImplSdlGLES2_NewFrame(SDL_Window* window);
IMGUI_API void        ImGui_ImplSdlGLES2_RenderDrawLists(ImDrawData* draw_data);

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void        ImGui_ImplSdlGLES2_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_ImplSdlGLES2_CreateDeviceObjects();

#endif // IMGUI_IMPL_SDL_ES2