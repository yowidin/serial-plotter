/**
 * @file   std_input_text.cpp
 * @author Dennis Sitelew
 * @date   Aug. 17, 2021
 */

#include <asp/ui/std_input_text.h>

namespace internal {

struct callback_data_t {
   std::string *data_;
   ImGuiInputTextCallback chain_cb_;
   void *chain_user_data_;
};

static int callback_t(ImGuiInputTextCallbackData *data) {
   auto user_data = static_cast<callback_data_t *>(data->UserData);
   if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
      // Resize string callback
      // If for some reason we refuse the new length (BufTextLen) and/or
      // capacity (BufSize) we need to set them back to what we want.
      auto str = user_data->data_;
      IM_ASSERT(data->Buf == str->c_str());
      str->resize(data->BufTextLen);
      data->Buf = (char *)str->c_str();
   } else if (user_data->chain_cb_) {
      // Forward to user callback, if any
      data->UserData = user_data->chain_user_data_;
      return user_data->chain_cb_(data);
   }
   return 0;
}

} // namespace internal

bool asp::ui::std_input_text(const char *label,
                             std::string &str,
                             ImGuiInputTextFlags flags,
                             ImGuiInputTextCallback callback,
                             void *user_data) {
   IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
   flags |= ImGuiInputTextFlags_CallbackResize;

   internal::callback_data_t cb_user_data{&str, callback, user_data};
   return ImGui::InputText(label, (char *)str.c_str(), str.capacity() + 1,
                           flags, internal::callback_t, &cb_user_data);
}