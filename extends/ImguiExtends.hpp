#ifndef __IMGUI_EXTENDS_HPP__DSVISUAL
#define __IMGUI_EXTENDS_HPP__DSVISUAL

#include <cmath>

#include <imgui.h>

// imgui extend
#include <imnodes.h>

namespace ImGuiExtends {
// ImNodes::GetNodeGridSpacePos(int id)
struct LNodeHelper {
   static int inputId(int id) { return id + 1; }
   static int outputId(int id) { return id + 2; }
   static int linkIdL(int id) { return id + 3; }
   static int linkIdR(int id) { return id + 4; }
};

static int DLNode(int nodeId, std::function<void ()> drawData = nullptr, float x = -1, float y = -1, bool updatePos = true) {
    ImNodes::BeginNode(nodeId);
        ImGui::Button("prev");
        ImNodes::BeginInputAttribute(LNodeHelper::inputId(nodeId));
        ImNodes::EndInputAttribute();
        ImGui::SameLine();
        ImNodes::BeginOutputAttribute(LNodeHelper::outputId(nodeId));
        ImNodes::EndOutputAttribute();
        ImGui::Button("next");

        if (drawData) drawData();
        ImNodes::SetNodeDraggable(nodeId, true);
        if (updatePos && x >= 0 && y >= 0)
            ImNodes::SetNodeEditorSpacePos(nodeId, {x, y});
    ImNodes::EndNode();

    return nodeId;
}


static void Link(const int id, const int start_attr_id, const int end_attr_id, bool connected = true) {

}

static void DrawArrowedLine(const ImVec2& start, const ImVec2& end) {
    ImGui::GetWindowDrawList()->AddLine(start, end, IM_COL32(255, 0, 0, 255), 2.0f);

    ImVec2 dir = ImVec2(end.x - start.x, end.y - start.y);
    float angle = atan2f(dir.y, dir.x);
    float arrowLength = 10.0f;

    auto p1 = ImVec2(cosf(angle + 3.141592f / 6) * arrowLength, sinf(angle + 3.141592f / 6) * arrowLength);
    ImVec2 arrowP1 = ImVec2(end.x - p1.x, end.y - p1.y);
    auto p2 = ImVec2(cosf(angle - 3.141592f / 6) * arrowLength, sinf(angle - 3.141592f / 6) * arrowLength);
    ImVec2 arrowP2 = ImVec2(end.x - p2.x, end.y - p2.y);
    ImGui::GetWindowDrawList()->AddLine(end, arrowP1, IM_COL32(255, 0, 0, 255), 2.0f);
    ImGui::GetWindowDrawList()->AddLine(end, arrowP2, IM_COL32(255, 0, 0, 255), 2.0f);
}

}
#endif