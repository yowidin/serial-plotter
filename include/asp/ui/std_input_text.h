/**
 * @file   std_input_text.h
 * @author Dennis Sitelew
 * @date   Aug. 17, 2021
 */
#ifndef INCLUDE_ASP_UI_STD_INPUT_TEXT_H
#define INCLUDE_ASP_UI_STD_INPUT_TEXT_H

#include <imgui.h>

#include <string>

namespace asp::ui {

bool std_input_text(const char *label,
                    std::string &str,
                    ImGuiInputTextFlags flags = 0,
                    ImGuiInputTextCallback callback = nullptr,
                    void *user_data = nullptr);

}

#endif /* INCLUDE_ASP_UI_STD_INPUT_TEXT_H */
