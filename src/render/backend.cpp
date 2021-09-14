/**
 * @file   backend.cpp
 * @author Dennis Sitelew
 * @date   Sep. 14, 2021
 */

#include <asp/render/backend.h>
#include <asp/render/opengl2.h>
#include <asp/render/opengl3.h>
#include <asp/render/gles2.h>
#include <asp/render/gles3.h>

#include <string>
#include <stdexcept>

using namespace asp::render;

std::unique_ptr<backend> backend::make(version v, frontend &f) {
   switch (v) {
      case version::opengl2:
         return std::make_unique<OpenGL2>(f);

      case version::opengl3:
         return std::make_unique<OpenGL3>(f);

      case version::gles2:
         return std::make_unique<GLES2>(f);

      case version::gles3:
         return std::make_unique<GLES3>(f);
   }

   throw std::runtime_error("Unexpected GL Version: " +
                            std::to_string(static_cast<int>(v)));
}
