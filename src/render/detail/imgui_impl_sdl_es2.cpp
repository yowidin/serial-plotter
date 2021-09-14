// ImGui SDL2 binding with OpenGL ES2 (based on SDL+GL3 example)
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture
// identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See
// main.cpp for an example of using this. If you use this binding you'll need to
// call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(),
// ImGui::Render() and ImGui_ImplXXXX_Shutdown(). If you are new to ImGui, see
// examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui
#include <asp/render/detail/imgui_impl_sdl_es2.h>

// SDL,GL3W
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <glad/glad.h>

// Data
static GLuint g_FontTexture = 0;
static int g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
static int g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
static int g_AttribLocationPosition = 0, g_AttribLocationUV = 0,
           g_AttribLocationColor = 0;
static unsigned int g_VboHandle = 0, /*g_VaoHandle = 0,*/ g_ElementsHandle = 0;

// This is the main rendering function that you have to implement and provide to
// ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure) If text
// or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by
// (0.5f,0.5f) or (0.375f,0.375f)
void ImGui_ImplSdlGLES2_RenderDrawLists(ImDrawData *draw_data) {
   // Avoid rendering when minimized, scale coordinates for retina displays
   // (screen coordinates != framebuffer coordinates)
   ImGuiIO &io = ImGui::GetIO();
   // Because of some weird handling of Android's virtual keyboard, we have to
   // check if the Backspace button is pressed
   const Uint8 *kbState = SDL_GetKeyboardState(NULL);
   if (!kbState[SDL_SCANCODE_BACKSPACE]) {
      io.KeysDown[SDLK_BACKSPACE] = 0;
   }
   int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
   int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
   if (fb_width == 0 || fb_height == 0)
      return;
   draw_data->ScaleClipRects(io.DisplayFramebufferScale);

   // Backup GL state
   GLint last_active_texture;
   glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
   glActiveTexture(GL_TEXTURE0);
   GLint last_program;
   glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
   GLint last_texture;
   glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
   GLint last_array_buffer;
   glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
   GLint last_element_array_buffer;
   glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
   // Note that your vertex buffer state is NOT saved, since es2 has no vertex
   // array objects
   // GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING,
   // &last_vertex_array);
   GLint last_blend_src_rgb;
   glGetIntegerv(GL_BLEND_SRC_RGB, &last_blend_src_rgb);
   GLint last_blend_dst_rgb;
   glGetIntegerv(GL_BLEND_DST_RGB, &last_blend_dst_rgb);
   GLint last_blend_src_alpha;
   glGetIntegerv(GL_BLEND_SRC_ALPHA, &last_blend_src_alpha);
   GLint last_blend_dst_alpha;
   glGetIntegerv(GL_BLEND_DST_ALPHA, &last_blend_dst_alpha);
   GLint last_blend_equation_rgb;
   glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
   GLint last_blend_equation_alpha;
   glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
   GLint last_viewport[4];
   glGetIntegerv(GL_VIEWPORT, last_viewport);
   GLint last_scissor_box[4];
   glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
   GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
   GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
   GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
   GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

   // Setup render state: alpha-blending enabled, no face culling, no depth
   // testing, scissor enabled
   glEnable(GL_BLEND);
   glBlendEquation(GL_FUNC_ADD);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_CULL_FACE);
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_SCISSOR_TEST);

   // Setup viewport, orthographic projection matrix
   glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
   const float ortho_projection[4][4] = {
       {2.0f / io.DisplaySize.x, 0.0f, 0.0f, 0.0f},
       {0.0f, 2.0f / -io.DisplaySize.y, 0.0f, 0.0f},
       {0.0f, 0.0f, -1.0f, 0.0f},
       {-1.0f, 1.0f, 0.0f, 1.0f},
   };
   glUseProgram(g_ShaderHandle);
   glUniform1i(g_AttribLocationTex, 0);
   glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE,
                      &ortho_projection[0][0]);
   // glBindVertexArray(g_VaoHandle);

   glEnableVertexAttribArray(g_AttribLocationPosition);
   glEnableVertexAttribArray(g_AttribLocationUV);
   glEnableVertexAttribArray(g_AttribLocationColor);

   glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
#define OFFSETOF(TYPE, ELEMENT) ((size_t) & (((TYPE *)0)->ELEMENT))
   glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE,
                         sizeof(ImDrawVert),
                         (GLvoid *)OFFSETOF(ImDrawVert, pos));
   glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE,
                         sizeof(ImDrawVert),
                         (GLvoid *)OFFSETOF(ImDrawVert, uv));
   glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                         sizeof(ImDrawVert),
                         (GLvoid *)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

   for (int n = 0; n < draw_data->CmdListsCount; n++) {
      const ImDrawList *cmd_list = draw_data->CmdLists[n];
      const ImDrawIdx *idx_buffer_offset = 0;

      glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
      glBufferData(GL_ARRAY_BUFFER,
                   (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert),
                   (const GLvoid *)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx),
                   (const GLvoid *)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

      for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
         const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];
         if (pcmd->UserCallback) {
            pcmd->UserCallback(cmd_list, pcmd);
         } else {
            glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
            glScissor((int)pcmd->ClipRect.x,
                      (int)(fb_height - pcmd->ClipRect.w),
                      (int)(pcmd->ClipRect.z - pcmd->ClipRect.x),
                      (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
            glDrawElements(
                GL_TRIANGLES, (GLsizei)pcmd->ElemCount,
                sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
                idx_buffer_offset);
         }
         idx_buffer_offset += pcmd->ElemCount;
      }
   }

   // Restore modified GL state
   glUseProgram(last_program);
   glBindTexture(GL_TEXTURE_2D, last_texture);
   glActiveTexture(last_active_texture);
   // glBindVertexArray(last_vertex_array);
   glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
   glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
   glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb,
                       last_blend_src_alpha, last_blend_dst_alpha);
   if (last_enable_blend)
      glEnable(GL_BLEND);
   else
      glDisable(GL_BLEND);
   if (last_enable_cull_face)
      glEnable(GL_CULL_FACE);
   else
      glDisable(GL_CULL_FACE);
   if (last_enable_depth_test)
      glEnable(GL_DEPTH_TEST);
   else
      glDisable(GL_DEPTH_TEST);
   if (last_enable_scissor_test)
      glEnable(GL_SCISSOR_TEST);
   else
      glDisable(GL_SCISSOR_TEST);
   glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2],
              (GLsizei)last_viewport[3]);
   glScissor(last_scissor_box[0], last_scissor_box[1],
             (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

static const char *ImGui_ImplSdlGLES2_GetClipboardText(void *) {
   return SDL_GetClipboardText();
}

static void ImGui_ImplSdlGLES2_SetClipboardText(void *, const char *text) {
   SDL_SetClipboardText(text);
}

void ImGui_ImplSdlGLES2_CreateFontsTexture() {
   // Build texture atlas
   ImGuiIO &io = ImGui::GetIO();
   unsigned char *pixels;
   int width, height;
   io.Fonts->GetTexDataAsRGBA32(
       &pixels, &width,
       &height); // Load as RGBA 32-bits for OpenGL3 demo because it is more
                 // likely to be compatible with user's existing shader.

   // Upload texture to graphics system
   GLint last_texture;
   glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
   glGenTextures(1, &g_FontTexture);
   glBindTexture(GL_TEXTURE_2D, g_FontTexture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   // gles2 does not support changing UNPACK_ROW_LENGTH, assume it's always 0?
   // glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                GL_UNSIGNED_BYTE, pixels);

   // Store our identifier
   io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

   // Restore state
   glBindTexture(GL_TEXTURE_2D, last_texture);
}

bool ImGui_ImplSdlGLES2_CreateDeviceObjects() {
   // Backup GL state
   GLint last_texture, last_array_buffer, last_vertex_array;
   glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
   glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
   // glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

   // Set version to 100 for gles2 shading language
   const GLchar *vertex_shader =
       "#version 100\n"
       "uniform mat4 ProjMtx;\n"
       "attribute vec2 Position;\n"
       "attribute vec2 UV;\n"
       "attribute vec4 Color;\n"
       "varying vec2 Frag_UV;\n"
       "varying vec4 Frag_Color;\n"
       "void main()\n"
       "{\n"
       "	Frag_UV = UV;\n"
       "	Frag_Color = Color;\n"
       "	gl_Position = ProjMtx * vec4(Position.xy,0.0,1.0);\n"
       "}\n";

   // You need to specify your precision in FS, according to ES2 shading
   // language
   const GLchar *fragment_shader =
       "#version 100\n"
       "precision mediump float;"
       "uniform sampler2D Texture;\n"
       "varying vec2 Frag_UV;\n"
       "varying vec4 Frag_Color;\n"
       "void main()\n"
       "{\n"
       "	gl_FragColor = Frag_Color * texture2D( Texture, Frag_UV.st);\n"
       "}\n";

   g_ShaderHandle = glCreateProgram();
   g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
   g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
   glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
   glCompileShader(g_VertHandle);
   glCompileShader(g_FragHandle);
   glAttachShader(g_ShaderHandle, g_VertHandle);
   glAttachShader(g_ShaderHandle, g_FragHandle);
   glLinkProgram(g_ShaderHandle);

   g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
   g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
   g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
   g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
   g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");

   glGenBuffers(1, &g_VboHandle);
   glGenBuffers(1, &g_ElementsHandle);

   // glGenVertexArrays(1, &g_VaoHandle);
   // glBindVertexArray(g_VaoHandle);
   glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
   glEnableVertexAttribArray(g_AttribLocationPosition);
   glEnableVertexAttribArray(g_AttribLocationUV);
   glEnableVertexAttribArray(g_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t) & (((TYPE *)0)->ELEMENT))
   glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE,
                         sizeof(ImDrawVert),
                         (GLvoid *)OFFSETOF(ImDrawVert, pos));
   glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE,
                         sizeof(ImDrawVert),
                         (GLvoid *)OFFSETOF(ImDrawVert, uv));
   glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                         sizeof(ImDrawVert),
                         (GLvoid *)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

   ImGui_ImplSdlGLES2_CreateFontsTexture();

   // Restore modified GL state
   glBindTexture(GL_TEXTURE_2D, last_texture);
   glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
   // glBindVertexArray(last_vertex_array);

   return true;
}

void ImGui_ImplSdlGLES2_InvalidateDeviceObjects() {
   // if (g_VaoHandle) glDeleteVertexArrays(1, &g_VaoHandle);
   if (g_VboHandle)
      glDeleteBuffers(1, &g_VboHandle);
   if (g_ElementsHandle)
      glDeleteBuffers(1, &g_ElementsHandle);
   /*g_VaoHandle = */ g_VboHandle = g_ElementsHandle = 0;

   if (g_ShaderHandle && g_VertHandle)
      glDetachShader(g_ShaderHandle, g_VertHandle);
   if (g_VertHandle)
      glDeleteShader(g_VertHandle);
   g_VertHandle = 0;

   if (g_ShaderHandle && g_FragHandle)
      glDetachShader(g_ShaderHandle, g_FragHandle);
   if (g_FragHandle)
      glDeleteShader(g_FragHandle);
   g_FragHandle = 0;

   if (g_ShaderHandle)
      glDeleteProgram(g_ShaderHandle);
   g_ShaderHandle = 0;

   if (g_FontTexture) {
      glDeleteTextures(1, &g_FontTexture);
      ImGui::GetIO().Fonts->TexID = 0;
      g_FontTexture = 0;
   }
}

bool ImGui_ImplSdlGLES2_Init() {
   ImGuiIO &io = ImGui::GetIO();
   io.BackendRendererName = "imgui_impl_gles2";
   return true;
}

void ImGui_ImplSdlGLES2_Shutdown() {
   ImGui_ImplSdlGLES2_InvalidateDeviceObjects();
}

void ImGui_ImplSdlGLES2_NewFrame(SDL_Window *window) {
   if (!g_FontTexture)
      ImGui_ImplSdlGLES2_CreateDeviceObjects();
}
