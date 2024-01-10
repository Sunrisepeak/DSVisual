#ifndef __HANIM_OBJECT_IMGUI_HPP__HANIM
#define __HANIM_OBJECT_IMGUI_HPP__HANIM

#include <Hanim.hpp>
#include <imgui.h>

namespace hanim {
namespace object {
namespace imgui {

class Rectangle : public hanim::HObjectTemplate {

public:
    Rectangle() : HObjectTemplate() {
        _mX = _mY = 0;
        _mW = _mH = 85;
        _mR = 153;
        _mG = _mB = 255;
        _mA = 255;
    }

protected: // interface impl
    void _render() override {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();

        ImVec2 clipRectMin = windowPos;
        ImVec2 clipRectMax = ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y);

        drawList->PushClipRect(clipRectMin, clipRectMax, true);

        drawList->AddRect({_mX, _mY}, {_mX + _mW, _mY + _mH}, IM_COL32(_mR * 255, _mG * 255, _mB * 255, _mA * 255));

        drawList->PopClipRect();
    }

};

}
}
}

#endif