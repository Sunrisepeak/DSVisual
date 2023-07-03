#ifndef __ARRAY_HPP__DSVISUAL
#define __ARRAY_HPP__DSVISUAL

#include <dstruct.hpp>

#include <dsv-core/dsvisual-core.hpp>

namespace dsvisual {

template <typename T, size_t N>
class Array : public dstruct::Array<T, N>, public Widget {

protected:
    using DStruct = dstruct::Array<T, N>;

public:
    Array() {
        _mName = std::string("Array-") + std::to_string(reinterpret_cast<size_t>(this));
        _mStartIndex = 0;
        _mHighlightIndex = -1;
    }

public: // for user
    T & operator[](int index) {
        if (index < 0)
            index = N + index;
        _mHighlightIndex = index;
        return DStruct::operator[](index);
    }

    T operator[](int index) const {
        if (index < 0)
            index = N + index;
        _mHighlightIndex = index;
        return DStruct::operator[](index);
    }

protected: // interface impl

    void draw_basic_info_impl() override {
        ImGui::Text("this: %p", this); ImGui::Separator();
        ImGui::Text("_mSize: %ld", N); ImGui::Separator();
        ImGui::Text("_mC Address: %p", this->_mC); ImGui::Separator();
    }

    void draw_visual_impl() override {
        ImGuiStyle& style = ImGui::GetStyle();
        const float buttonWidth = 50.0f;
        const float buttonHeight = 50.0f;
        const float buttonSpacing = 2.0f;
        const float windowWidth = ImGui::GetWindowWidth();
        auto oldSpacing = style.ItemSpacing;

        style.ItemSpacing = ImVec2(10, 10);
    
        for (int i = _mStartIndex; i < N; i++) {
            auto remindWidth = windowWidth - ImGui::GetCursorPosX(); 
            if (i > 0 && remindWidth < buttonWidth + buttonSpacing) {
                ImGui::Button(std::to_string((*this)[i]).c_str(), ImVec2(remindWidth - buttonSpacing, buttonHeight));
                break;
            } else {
                if (i == _mHighlightIndex) {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.0f, 0.0f, 0.7f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
                }
                ImGui::Button(std::to_string(DStruct::operator[](i)).c_str(), ImVec2(buttonWidth, buttonHeight));
                if (i == _mHighlightIndex) { ImGui::PopStyleColor(3); /*_mHighlightIndex = -1;*/ }
                ImGui::SameLine(0, buttonSpacing);
            }
        }

        ImGui::Separator();
        style.ItemSpacing = oldSpacing;

    }


    void draw_control_impl() override {
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth());
        ImGui::SliderInt("", &_mStartIndex, 0, N - 1, "Start Index %d"); ImGui::Separator();
    }
protected:
    int _mHighlightIndex;
    int _mStartIndex;
};

}

#endif