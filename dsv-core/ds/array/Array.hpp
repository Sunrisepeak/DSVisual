#ifndef __ARRAY_HPP__DSVISUAL
#define __ARRAY_HPP__DSVISUAL

#include <initializer_list>

#include <dstruct.hpp>

#include <dsv-core/dsvisual-core.h>
#include <dsv-core/ds/ds-base.hpp>

namespace dsvisual {


namespace ds {

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
        _mMaxVal = N;
        _mDataVisualExtend = false;
    }

    Array(std::initializer_list<T> &&list) : Array() {
        auto it = begin();
        for (auto listIt = list.begin(); it != end() && listIt != list.end(); listIt++, it++) {
            *it = *listIt;
        }
    }

public: // for user
    T & operator[](int index) {
        if (index < 0)
            index = N + index;
        _mModifiedPos = index;
        _animation();
        return DStruct::operator[](index);
    }

    T operator[](int index) const {
        if (index < 0)
            index = N + index;
        _mIteratorPos = index;
        _animation();
        return DStruct::operator[](index);
    }

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

public: // animation
    void setDataVisualExtend(bool dataVisualExtend) {
        _mDataVisualExtend = dataVisualExtend;
    }


protected:
    void _updateIterator(const _PrimitiveIterator<T, Array> &it, bool changeFlag = false) {
        _mIteratorPos = dstruct::distance(begin(), it);
        if (changeFlag) { _mModifiedPos = _mIteratorPos; }
        else _mModifiedPos = -1;
        _animation();
    }

    void _animation() {
        //std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    void _dataVisualBaseImpl() {
        ImGuiStyle& style = ImGui::GetStyle();
        const float buttonSpacing = 2.0f;
        const float windowWidth = ImGui::GetWindowWidth();
        auto oldSpacing = style.ItemSpacing;

        buttonWidth = buttonHeight = windowWidth / 20;

        style.ItemSpacing = ImVec2(buttonWidth, buttonHeight);

        ImGui::Separator();
        for (int i = _mStartIndex; i < N; i++) {            
            auto remindWidth = windowWidth - ImGui::GetCursorPosX();

            if (i > 0 && remindWidth < buttonWidth + buttonSpacing) {
               buttonWidth = remindWidth - buttonSpacing;
               if (buttonWidth <= 0) {
                    ImGui::Separator();
                    break;
               }
            }

            auto highlight = style.Colors[ImGuiCol_Button];

            if (i == _mIteratorPos) highlight = ImVec4(0.5f, 1.0f, 0.5f, 0.3f);
            if (i == _mModifiedPos) highlight = ImVec4(0.7f, 0.0f, 0.0f, 0.7f);

            ImGui::PushStyleColor(ImGuiCol_Button, highlight);
            std::string text = std::to_string(DStruct::operator[](i));
            ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
            if (textSize.x > buttonWidth) buttonWidth = textSize.x;
            ImGui::Button(text.c_str(), ImVec2(buttonWidth + 10, buttonHeight + 20));
            if (i == _mModifiedPos || i == _mIteratorPos) { ImGui::PopStyleColor(1); /*_mModifiedPos = -1;*/ }

            if (i < N - 1) ImGui::SameLine(0, buttonSpacing);

        }

        style.ItemSpacing = oldSpacing;
    }

    void _dataVisualExtendImpl() {

        auto func = [](void *data, int x) -> float {
            T *arr = static_cast<T *>(data);
            return arr[x] * 1.0f; 
        };

        ImGui::PlotHistogram("", func, this->_mC, N, _mStartIndex,
            NULL, FLT_MAX, FLT_MAX, ImVec2(ImGui::GetWindowWidth(), buttonHeight * 2));
        ImGui::PlotLines("", func, this->_mC, N, _mStartIndex,
            NULL, FLT_MAX, FLT_MAX, ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() / 4));
    }

protected: // interface impl

    void _drawBasicInfoImpl() override {
        ImGui::Text("this: %p", this); ImGui::Separator();
        ImGui::Text("_mSize: %ld", N); ImGui::Separator();
        ImGui::Text("_mC Address: %p", this->_mC); ImGui::Separator();
    }

    void _drawVisualImpl() override {
        _dataVisualBaseImpl(); ImGui::Separator();
        if (_mDataVisualExtend)
            _dataVisualExtendImpl(); ImGui::Separator();
    }

    void _drawControlImpl() override {
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth());
        ImGui::SliderInt("", &_mStartIndex, 0, N - 1, "Start Index %d"); ImGui::Separator();
    }

protected:
    // base data
    float _mMaxVal;
    float buttonWidth, buttonHeight;

    // animation
    int _mModifiedPos;
    int _mIteratorPos;

    // data visual control
    int _mStartIndex;
    bool _mDataVisualExtend;
};

}

}

#endif