/**
 * @file   data.cpp
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */

#include <asp/inputs/data.h>
#include <asp/options.h>
#include <asp/ui/logs.h>
#include <asp/ui/std_input_text.h>

#include <imgui.h>
#include <implot.h>

using namespace asp::inputs;
using namespace std::string_literals;

////////////////////////////////////////////////////////////////////////////////
/// Class: data::options
////////////////////////////////////////////////////////////////////////////////
asp::po_desc_t data::options::prepare() {
   namespace po = boost::program_options;

   po::options_description od("Data Options");

   // clang-format off
   od.add_options()
       ("name-filter,n", po::value<std::string>()->default_value("^.*$"),
         "Name filter regex. Entries, not passing this filter will be ignored")
       ("graph-filter,g", po::value<std::string>()->default_value("^.*$"),
        "Graph filter regex. Entries, not passing this filter will not be displayed")
       ("follow,f",
        "Follow the head of the graph, displaying only the last --follow-window entries")
       ("follow-window", po::value<int>()->default_value(300),
        "Number of trailing entries to be displayed (has no effect without the --follow flag)")
       ;
   // clang-format on

   return od;
}

data::options data::options::load(po_vars_t &vm) {
   data::options opts;
   opts.name_filter = vm["name-filter"].as<std::string>();
   opts.graph_filter = vm["graph-filter"].as<std::string>();
   opts.follow = vm.count("follow");
   opts.follow_window = vm["follow-window"].as<int>();
   return opts;
}

////////////////////////////////////////////////////////////////////////////////
/// Class: data
////////////////////////////////////////////////////////////////////////////////
data::data(const asp::options &opts, ui::logs &logs)
   : opts_{opts.data}
   , logs_{&logs}
   , name_regex_{opts_.name_filter}
   , graph_regex_{opts_.graph_filter} {
   // Nothing to do here
}

void data::add_raw_entry(const std::string &entry) {
   std::string name{};
   std::string value{};
   std::string *target = &name;

   bool changed = false;

   auto add = [&] {
      try {
         if (std::regex_match(name, name_regex_)) {
            auto result = std::stof(value);
            plot_data_[name].push_back(result);
            changed = true;
         }

         name.clear();
         value.clear();
         target = &name;
      } catch (const std::exception &e) {
         logs_->add("Error parsing a value (", e.what(), "): ", value);
      }
   };

   // (name: value,?)+
   for (const auto ch : entry) {
      if (ch == ':') {
         target = &value;
         continue;
      }

      if (ch == ',') {
         add();
         continue;
      }

      (*target) += ch;
   }

   if (!target->empty()) {
      if (target == &value) {
         add();
      } else {
         logs_->add("Invalid string: ", entry);
      }
   }

   if (changed && data_change_cb_) {
      data_change_cb_(*this);
   }
}

void data::draw() {
   draw_settings();
   draw_plot();
}

void data::draw_settings() {
   if (!error_message_.empty() && !collapsed_) {
      ImGui::SetNextItemOpen(true);
   }

   // Update limits, even if data settings are collapsed
   update_limits();

   if (!ImGui::CollapsingHeader("Data")) {
      collapsed_ = true;
      return;
   }

   auto set_regex = [&](auto &ex, auto &text) {
      try {
         ex.assign(text);
         return true;
      } catch (const std::exception &e) {
         error_message_ = "Filter error: "s + e.what();
         collapsed_ = false;
         return false;
      }
   };

   if (!ImGui::BeginTable("data##options", 2,
                          ImGuiTableFlags_SizingFixedFit)) {
      return;
   }

   auto next = [] { ImGui::TableNextColumn(); };

   static auto name_error = false;
   static auto graph_error = false;

   next();
   if (ui::std_input_text("Name filter", opts_.name_filter)) {
      name_error = !set_regex(name_regex_, opts_.name_filter);
   }

   next();
   ImGui::Checkbox("Follow", &opts_.follow);
   if (opts_.follow) {
      ImGui::SameLine();
      ImGui::DragInt("Window", &opts_.follow_window, 10.0f, 100, 50000);
   }

   next();
   if (ui::std_input_text("Graph filter", opts_.graph_filter)) {
      graph_error = !set_regex(graph_regex_, opts_.graph_filter);
   }

   if (!name_error && !graph_error) {
      error_message_.clear();
   }

   if (opts_.follow) {
      int max_entries = [&]() {
         std::size_t result = 0;
         for (const auto &kv : plot_data_) {
            result = std::max(kv.second.size(), result);
         }
         return static_cast<int>(result);
      }();

      int min_x, max_x = max_entries;
      if (max_entries < opts_.follow_window) {
         min_x = 0;
      } else {
         min_x = max_entries - opts_.follow_window;
      }
      ImPlot::SetNextPlotLimitsX(min_x, max_x, ImGuiCond_Always);
   }

   next();
   if (ImGui::Button("Clear")) {
      plot_data_.clear();
   }

   ImGui::EndTable();

   if (!error_message_.empty()) {
      ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, "%s",
                         error_message_.c_str());
   }
}

void data::update_limits() {
   plot_flags_ = ImPlotFlags_NoTitle;

   // Axis flags
   x_axis_flags_ = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoLabel;
   y_axis_flags_ = ImPlotAxisFlags_NoLabel;
   if (opts_.follow) {
      y_axis_flags_ |= (ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit);
   }

   if (opts_.follow) {
      int max_entries = [&]() {
         std::size_t result = 0;
         for (const auto &kv : plot_data_) {
            result = std::max(kv.second.size(), result);
         }
         return static_cast<int>(result);
      }();

      max_x_ = max_entries;
      if (max_entries < opts_.follow_window) {
         min_x_ = 0;
      } else {
         min_x_ = max_entries - opts_.follow_window;
      }
   }
}

void data::draw_plot() {
   if (opts_.follow) {
      ImPlot::SetNextPlotLimitsX(min_x_, max_x_, ImGuiCond_Always);
   }

   if (ImPlot::BeginPlot("Data", "Time", "Value", {-1, -1}, plot_flags_,
                         x_axis_flags_, y_axis_flags_)) {
      for (const auto &kv : plot_data_) {
         const auto &name = kv.first;
         const auto &values = kv.second;
         if (!std::regex_match(name, graph_regex_)) {
            continue;
         }

         ImPlot::PlotLine(name.c_str(), values.data(),
                          static_cast<int>(values.size()));
      }

      if (draw_plot_cb_) {
         draw_plot_cb_(*this);
      }

      ImPlot::EndPlot();
   }
}
