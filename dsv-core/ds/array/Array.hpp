#ifndef __ARRAY_HPP__DSVISUAL
#define __ARRAY_HPP__DSVISUAL

#include <dstruct.hpp>

#include <dsv-core/dsvisual-core.hpp>

namespace dsvisual {

template <typename T, size_t N>
class Array : private dstruct::Array<T, N>, public Widget {
    friend class _PrimitiveIterator<T, Array>;
protected:
    using DStruct = dstruct::Array<T, N>;

public: // bigfive
    Array() {
        _mName = std::string("Array-") + std::to_string(reinterpret_cast<size_t>(this));
        _mStartIndex = 0;
        _mIteratorPos = -1;
        _mModifiedPos = -1;
    }

public: // for user
    T & operator[](int index) {
        if (index < 0)
            index = N + index;
        _mModifiedPos = index;
        return DStruct::operator[](index);
    }

    T operator[](int index) const {
        if (index < 0)
            index = N + index;
        _mModifiedPos = index;
        return DStruct::operator[](index);
    }

public:
    public: // base op
    size_t size() const {
        return N;
    }

    _PrimitiveIterator<T, Array> begin() {
        return { DStruct::_mC, this };
    }

    _PrimitiveIterator<const T, Array> begin() const {
        return { DStruct::_mC, this };
    }

    _PrimitiveIterator<T, Array> end() {
        return { DStruct::_mC + N, this };
    }

    _PrimitiveIterator<const T, Array> end() const {
        return { DStruct::_mC + N, this };
    }

protected:
    void _updateIterator(const _PrimitiveIterator<T, Array> &it, bool changeFlag = false) {
        _mIteratorPos = dstruct::distance(begin(), it);
        if (changeFlag) _mModifiedPos = _mIteratorPos;
        else _mModifiedPos = -1;
    }

protected: // interface impl

    void _drawBasicInfoImpl() override {
        ImGui::Text("this: %p", this); ImGui::Separator();
        ImGui::Text("_mSize: %ld", N); ImGui::Separator();
        ImGui::Text("_mC Address: %p", this->_mC); ImGui::Separator();
    }

    void _drawVisualImpl() override {
        ImGuiStyle& style = ImGui::GetStyle();
        float buttonWidth = 50.0f;
        const float buttonHeight = 50.0f;
        const float buttonSpacing = 2.0f;
        const float windowWidth = ImGui::GetWindowWidth();
        auto oldSpacing = style.ItemSpacing;

        style.ItemSpacing = ImVec2(10, 10);
    
        for (int i = _mStartIndex; i < N; i++) {
            auto remindWidth = windowWidth - ImGui::GetCursorPosX();

            if (i > 0 && remindWidth < buttonWidth + buttonSpacing) {
               buttonWidth = remindWidth - buttonSpacing;
               if (buttonWidth <= 0) break;
            }

            auto highlight = style.Colors[ImGuiCol_Button];

            if (i == _mIteratorPos) highlight = ImVec4(0.5f, 1.0f, 0.5f, 0.3f);
            if (i == _mModifiedPos) highlight = ImVec4(0.7f, 0.0f, 0.0f, 0.7f);

            ImGui::PushStyleColor(ImGuiCol_Button, highlight);
            //ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
            //ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
            ImGui::Button(std::to_string(DStruct::operator[](i)).c_str(), ImVec2(buttonWidth, buttonHeight));
            if (i == _mModifiedPos || i == _mIteratorPos) { ImGui::PopStyleColor(1); /*_mModifiedPos = -1;*/ }

            ImGui::SameLine(0, buttonSpacing);
        }

        ImGui::Separator();
        style.ItemSpacing = oldSpacing;

    }


    void _drawControlImpl() override {
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth());
        ImGui::SliderInt("", &_mStartIndex, 0, N - 1, "Start Index %d"); ImGui::Separator();
    }
protected:
    int _mModifiedPos;
    int _mIteratorPos;
    int _mStartIndex;
};

}

#endif