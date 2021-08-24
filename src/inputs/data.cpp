/**
 * @file   data.cpp
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */

#include <asp/inputs/data.h>

#include <imgui.h>
#include <implot.h>

using namespace asp::inputs;

data::data(ui::logs &logs, ui::logs &raw)
   : logs_{&logs}
   , raw_{&raw} {
   // Nothing to do here
}

void data::draw() {
   // Plot flags
   ImPlotFlags plot_flags = ImPlotFlags_NoTitle;

   ImGui::Checkbox("Crosshairs", &cross_hair_);
   if (cross_hair_) {
      plot_flags |= ImPlotFlags_Crosshairs;
   }

   // Axis flags
   const auto x_flags = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoLabel;
   ImPlotAxisFlags y_flags = ImPlotAxisFlags_NoLabel;

   ImGui::SameLine();
   ImGui::Checkbox("Follow", &follow_);
   const ImPlotAxisFlags follow_flags =
       ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit;
   if (follow_) {
      y_flags |= follow_flags;

      ImGui::SameLine();
      ImGui::DragInt("Range", &follow_interval_, 10.0f, 100, 50000);

      int max_entries = [&]() {
        std::size_t result = 0;
        for (const auto &kv : plot_data_) {
           result = std::max(kv.second.size(), result);
        }
        return static_cast<int>(result);
      }();

      int min_x, max_x = max_entries;
      if (max_entries < follow_interval_) {
         min_x = 0;
      } else {
         min_x = max_entries - follow_interval_;
      }
      ImPlot::SetNextPlotLimitsX(min_x, max_x, ImGuiCond_Always);
   }

   if (ImPlot::BeginPlot("Data", "Time", "Value", {-1, -1}, plot_flags, x_flags,
                         y_flags)) {
      for (const auto &kv : plot_data_) {
         const auto &name = kv.first;
         const auto &values = kv.second;
         ImPlot::PlotLine(name.c_str(), values.data(),
                          static_cast<int>(values.size()));
      }

      ImPlot::EndPlot();
   }
}

void data::add_raw_entry(const std::string &entry) {
   raw_->add_entry(entry);

   std::string name{};
   std::string value{};
   std::string *target = &name;

   auto add = [&] {
      try {
         auto result = std::stof(value);
         plot_data_[name].push_back(result);
         name.clear();
         value.clear();
         target = &name;
      } catch (const std::exception &e) {
        logs_->add("Error parsing a value (", e.what(), "): ", value);
      }
   };

   // (name: value,)+
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
}