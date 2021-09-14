/**
 * @file   gles3.h
 * @author Dennis Sitelew
 * @date   Sep. 14, 2021
 */
#ifndef INCLUDE_ASP_RENDER_GLES3_H
#define INCLUDE_ASP_RENDER_GLES3_H

#include <asp/render/backend.h>

namespace asp::render {

class GLES3 : public backend {
public:
   using backend::backend;
   ~GLES3();

public:
   void init() override;
   void new_frame() override;
   void render() override;

   version get_version() override { return version::gles3; }
};

} // namespace asp::render

#endif /* INCLUDE_ASP_RENDER_GLES3_H */
