/**
 * @file   opengl2.h
 * @author Dennis Sitelew
 * @date   Sep. 14, 2021
 */
#ifndef INCLUDE_ASP_RENDER_OPENGL2_H
#define INCLUDE_ASP_RENDER_OPENGL2_H

#include <asp/render/backend.h>

namespace asp::render {

class OpenGL2 : public backend {
public:
   using backend::backend;
   ~OpenGL2();

public:
   void init() override;
   void new_frame() override;
   void render() override;

   version get_version() override { return version::opengl2; }
};

} // namespace asp::render

#endif /* INCLUDE_ASP_RENDER_OPENGL2_H */
