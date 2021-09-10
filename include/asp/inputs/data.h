/**
 * @file   data.h
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */
#ifndef INCLUDE_ASP_INPUTS_DATA_H
#define INCLUDE_ASP_INPUTS_DATA_H

#include <asp/types.h>
#include <asp/ui/drawable.h>

#include <string>
#include <unordered_map>
#include <vector>
#include <regex>
#include <functional>

namespace asp {

struct options;

namespace ui {
class logs;
}

namespace inputs {

class data : public ui::drawable {
public:
   struct options {
      std::string name_filter{};
      std::string graph_filter{};
      bool follow{};
      int follow_window{};

      static po_desc_t prepare();
      static options load(po_vars_t &vm);
   };

   using data_change_cb = std::function<void(data &)>;
   using draw_plot_cb = std::function<void(data &)>;
   using container_t = std::unordered_map<std::string, std::vector<float>>;

public:
   data(const asp::options &opts, ui::logs &logs);

public:
   void draw() override;
   void add_raw_entry(const std::string &entry);

   void set_data_change_callback(data_change_cb cb) {
      std::swap(cb, data_change_cb_);
   }

   void set_draw_plot_callback(draw_plot_cb cb) {
      std::swap(cb, draw_plot_cb_);
   }

   container_t &get_plot_data() {
      return plot_data_;
   }

   ui::logs &get_logs() {
      return *logs_;
   }

   data::options &get_options() {
      return opts_;
   }

private:
   data::options opts_;
   ui::logs *logs_;
   container_t plot_data_;

   std::regex name_regex_;
   std::regex graph_regex_;

   std::string error_message_{};

   data_change_cb data_change_cb_{};
   draw_plot_cb draw_plot_cb_{};
};

} // namespace inputs
} // namespace asp

#endif /* INCLUDE_ASP_INPUTS_DATA_H */
