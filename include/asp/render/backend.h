/**
 * @file   backend.h
 * @author Dennis Sitelew
 * @date   Sep. 14, 2021
 */
#ifndef INCLUDE_ASP_RENDER_BACKEND_H
#define INCLUDE_ASP_RENDER_BACKEND_H

#include <memory>

namespace asp::render {

enum class version { opengl2 = 2, opengl3 = 3, gles2 = 20, gles3 = 30 };

class frontend;

class backend {
public:
   backend(frontend &v)
      : frontend_{&v} {}

   virtual ~backend() = default;

public:
   virtual void init() = 0;
   virtual void new_frame() = 0;
   virtual void render() = 0;
   virtual version get_version() = 0;

   static std::unique_ptr<backend> make(version v, frontend &f);

protected:
   frontend *frontend_;
};

} // namespace asp::render

#endif /* INCLUDE_ASP_RENDER_BACKEND_H */
