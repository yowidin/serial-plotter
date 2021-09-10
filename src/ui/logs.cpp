/**
 * @file   logs.cpp
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */

#include <asp/ui/logs.h>

#include <imgui.h>

using namespace asp::ui;

logs::logs()
   : auto_scroll_id_("Autoscroll##logs")
   , clear_id_("Clear##logs")
   , child_id_("Logs##logs")
   , copy_id_("Copy##logs") {
   // Nothing to do here
}

void logs::draw() {
   if (!ImGui::CollapsingHeader("Logs##logs")) {
      return;
   }

   ImGui::Checkbox(auto_scroll_id_.c_str(), &auto_scroll_);
   ImGui::SameLine();
   if (ImGui::Button(clear_id_.c_str())) {
      logs_.clear();
   }

   ImGui::SameLine();
   bool copy_to_clipboard = ImGui::Button(copy_id_.c_str());

   const float height = ImGui::GetTextLineHeightWithSpacing() * 8;

   ImGui::BeginChild(child_id_.c_str(), ImVec2(0, height), true,
                     ImGuiWindowFlags_HorizontalScrollbar);
   ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

   if (copy_to_clipboard) {
      ImGui::LogToClipboard();
   }

   for (const auto &line : logs_) {
      ImGui::TextUnformatted(line.c_str());
   }

   if (copy_to_clipboard) {
      ImGui::LogFinish();
   }

   if (auto_scroll_) {
      ImGui::SetScrollHereY(1.0f);
   }

   ImGui::PopStyleVar();
   ImGui::EndChild();
}

void logs::add_entry(const std::string &entry) {
   logs_.push_back(entry);
}
