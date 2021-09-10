/**
 * @file   data.cpp
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */

#include <asp/inputs/data.h>
#include <asp/ui/logs.h>
#include <asp/options.h>
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
   auto set_regex = [&] (auto &ex, auto &text) {
     try {
        ex.assign(text);
        return true;
     } catch (const std::exception &e) {
        error_message_ = "Filter error: "s + e.what();
        return false;
     }
   };

   static auto name_error = false;
   static auto graph_error = false;
   if (ui::std_input_text("Name filter", opts_.name_filter)) {
      name_error = !set_regex(name_regex_, opts_.name_filter);
   }

   ImGui::SameLine();
   if (ui::std_input_text("Graph filter", opts_.graph_filter)) {
      graph_error = !set_regex(graph_regex_, opts_.graph_filter);
   }

   if (!name_error && !graph_error) {
      error_message_.clear();
   }

   // Plot flags
   ImGui::SameLine();
   ImPlotFlags plot_flags = ImPlotFlags_NoTitle;

   // Axis flags
   const auto x_flags = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoLabel;
   ImPlotAxisFlags y_flags = ImPlotAxisFlags_NoLabel;

   ImGui::SameLine();
   ImGui::Checkbox("Follow", &opts_.follow);
   const ImPlotAxisFlags follow_flags =
       ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit;
   if (opts_.follow) {
      y_flags |= follow_flags;

      ImGui::SameLine();
      ImGui::DragInt("Window", &opts_.follow_window, 10.0f, 100, 50000);

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

   if (!error_message_.empty()) {
      ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, "%s",
                         error_message_.c_str());
   }

   if (ImPlot::BeginPlot("Data", "Time", "Value", {-1, -1}, plot_flags, x_flags,
                         y_flags)) {
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