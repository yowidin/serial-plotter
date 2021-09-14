/**
 * @file   gles2.h
 * @author Dennis Sitelew
 * @date   Sep. 14, 2021
 */
#ifndef INCLUDE_ASP_RENDER_GLES2_H
#define INCLUDE_ASP_RENDER_GLES2_H

#include <asp/render/backend.h>

namespace asp::render {

class GLES2 : public backend {
public:
   using backend::backend;
   ~GLES2();

public:
   void init() override;
   void new_frame() override;
   void render() override;

   version get_version() override { return version::gles2; }
};

} // namespace asp::render

#endif /* INCLUDE_ASP_RENDER_GLES2_H */
