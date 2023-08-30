#ifndef __IMGUI_EXTENDS_HPP__DSVISUAL
#define __IMGUI_EXTENDS_HPP__DSVISUAL

#include <cmath>

#include <imgui.h>

// imgui extend
#include <imnodes.h>

struct LNodeInterface {
    LNodeInterface() {
        _mId = reinterpret_cast<decltype(_mId)>(this);
    }

    int id() {
        return _mId;
    }

    int inputId() {
        return _mId + 1;
    }

    int outputId() {
        return _mId + 2;
    }

    int linkIdL() {
        return _mId + 3;
    }

    int linkIdR() {
        return _mId + 4;
    }

protected:
    unsigned long long _mId; // sizeof(int) support id/inputId/outputId
};

struct SLNode : public LNodeInterface {
    SLNode(int nodeId, float x = 0, float y = 0, std::function<void ()> drawData = nullptr) {
        _mId = nodeId;
        ImNodes::BeginNode(id());
            ImNodes::BeginInputAttribute(inputId());
            ImGui::Text("");
            ImNodes::EndInputAttribute();
            ImGui::SameLine(); ImGui::Button("next"); ImGui::SameLine();
            ImNodes::BeginOutputAttribute(outputId());
            ImGui::Text("");
            ImNodes::EndOutputAttribute();

            if (drawData) {
                drawData();
            }
        ImNodes::EndNode();
    }
};

struct DLNode : public LNodeInterface {
    DLNode(int nodeId, float x = -1, float y = -1, std::function<void ()> drawData = nullptr) {
        _mId = nodeId;
        ImNodes::BeginNode(id());
            ImGui::Button("prev");
            ImNodes::BeginInputAttribute(inputId());
            ImNodes::EndInputAttribute();
            ImGui::SameLine();
            ImNodes::BeginOutputAttribute(outputId());
            ImNodes::EndOutputAttribute();
            ImGui::Button("next");

            if (drawData) {
                drawData();
            }
        ImNodes::EndNode();

        if (x >= 0 && y >= 0)
            ImNodes::SetNodeEditorSpacePos(id(), {x, y});
        
        ImNodes::SetNodeDraggable(id(), true);
    }
};


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

#endif