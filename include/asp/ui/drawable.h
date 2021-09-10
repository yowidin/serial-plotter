/**
 * @file   drawable.h
 * @author Dennis Sitelew
 * @date   Sep. 09, 2021
 */
#ifndef INCLUDE_ASP_UI_DRAWABLE_H
#define INCLUDE_ASP_UI_DRAWABLE_H

namespace asp::ui {

class drawable {
public:
   virtual ~drawable() = default;

public:
   virtual void draw() = 0;
};

} // namespace asp::ui

#endif /* INCLUDE_ASP_UI_DRAWABLE_H */
