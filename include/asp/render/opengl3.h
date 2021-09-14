/**
 * @file   opengl3.h
 * @author Dennis Sitelew
 * @date   Sep. 14, 2021
 */
#ifndef INCLUDE_ASP_RENDER_OPENGL3_H
#define INCLUDE_ASP_RENDER_OPENGL3_H

#include <asp/render/backend.h>

namespace asp::render {

class OpenGL3 : public backend {
public:
   using backend::backend;
   ~OpenGL3();

public:
   void init() override;
   void new_frame() override;
   void render() override;

   version get_version() override { return version::opengl3; }
};

} // namespace asp::render

#endif /* INCLUDE_ASP_RENDER_OPENGL3_H */
