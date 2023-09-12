

#ifndef __HANIM_EXTENDS_HPP__DSVISUAL
#define __HANIM_EXTENDS_HPP__DSVISUAL

#include <Hanim.hpp>

#include <imgui.h>

namespace hanim {
// Animate
namespace animate {
namespace dsvisual {
struct InsertAnim : public hanim::ComposeAnim {
    InsertAnim(float y, float offset, int frameNumbers = 60) {
        // 1 : 1 : 1 : 1
        int subF = frameNumbers / 4;
        hanim::ComposeAnim::move(-1, 0, -1, y / 2)
            .setFrameNums(subF);
        setFrameTrackIndex(0);

        hanim::ComposeAnim::move(0, -1, offset, -1)
            .setFrameNums(subF);
        setFrameTrackIndex(subF);

        hanim::ComposeAnim::scale(0, 1) // link anim
            .setFrameNums(subF);
        setFrameTrackIndex(2 * subF);

        hanim::ComposeAnim::move(-1, y / 2 + 1, -1, y)
            .setFrameNums(subF);
        setFrameTrackIndex(3 * subF); // TODO/Bug?: check finished status

        hanim::ComposeAnim::alpha(0, 150)
            .setFrameNums(frameNumbers);
        setFrameTrackIndex(0);

        setFrameNums(frameNumbers);
    }
};

struct DeleteAnim : public hanim::ComposeAnim {
    DeleteAnim(float deltaY, int frameNumbers = 60) {
        int subF = frameNumbers / 5;
        // step1: move down
        hanim::ComposeAnim::move(-1, 0, -1, deltaY)
            .setFrameNums(subF);
        setFrameTrackIndex(0);

        auto customAnimTemplate = hanim::InterpolationAnim(
            hanim::InterpolationAnim::IAType::CUSTOM,
            { 0 }, { 1 },
            subF
        );

        // step2: prev -> next
        customAnimTemplate.setStartFrame({0});
        customAnimTemplate.setEndFrame({1});
        addAnim(customAnimTemplate, subF);

        // step3: prev <- next
        customAnimTemplate.setStartFrame({1});
        customAnimTemplate.setEndFrame({2});
        addAnim(customAnimTemplate, subF * 2);

        // step4: prev <-x- curr -x-> next
        customAnimTemplate.setStartFrame({2});
        customAnimTemplate.setEndFrame({3});
        addAnim(customAnimTemplate, subF * 3);

        // step5: fade-out and move-L
        customAnimTemplate.setStartFrame({3});
        customAnimTemplate.setEndFrame({4});
        addAnim(customAnimTemplate, subF * 4);

        hanim::ComposeAnim::alpha(255, 0)
            .setFrameNums(subF);
        setFrameTrackIndex(subF * 4);

        setFrameNums(frameNumbers);
    }
};

}
}

// Object
namespace object {

namespace dsvisual {

class Node : public hanim::HObjectTemplate {
public:
    Node(int id) : HObjectTemplate(), __mId { id }, __mUpdatePos { true }, __mVisible { true }  { }
    Node & operator=(const Node &node) {
        __mId = node.__mId;
        __mUpdatePos = node.__mUpdatePos;
        __mVisible = node.__mVisible;
    }
public:
    // -1: <-
    // 0 : <->
    // 1 : ->
    static void connect(const Node &node1, const Node &node2, int flag = 0) {
        if (flag == 1)
            ImNodes::Link(node1.linkIdR(), node1.outputId(), node2.inputId());
        else if (flag == -1)
            ImNodes::Link(node2.linkIdL(), node2.inputId(), node1.outputId());
        else {
            ImNodes::Link(node1.linkIdR(), node1.outputId(), node2.inputId());
            ImNodes::Link(node2.linkIdL(), node2.inputId(), node1.outputId());
        }
    }

    static void disconnect(const Node &node1, const Node &node2, int flag = 0) {
        if (flag == 1)
            ImNodes::Link(node1.linkIdR(), 0, 0);
        else if (flag == -1)
            ImNodes::Link(node2.linkIdL(), 0, 0);
        else {
            ImNodes::Link(node1.linkIdR(), 0, 0);
            ImNodes::Link(node2.linkIdL(), 0, 0);
        }
    }
public:
    int id() const { return __mId; }
    int inputId() const { return __mId + 1; }
    int outputId() const { return __mId + 2; }
    int linkIdL() const { return __mId + 3; }
    int linkIdR() const { return __mId + 4; }
public:
    void setId(int id) { __mId = id; };
    void setUpdatePos(bool update = true) {
        __mUpdatePos = update;
    }
    void setVisible(bool visible) {
        __mVisible = visible;
    }
public:
    void render(std::function<void ()> drawData = nullptr) {

        if (!__mVisible) return;

        bool colorEnable = false;
        if (_mR >= 0 && _mG >= 0 && _mB >= 0) { // TODO: optimize _mA and (r,g,b)
            ImNodes::PushColorStyle(ImNodesCol_NodeBackground, IM_COL32(_mR, _mG, _mB, _mA < 0 ? 255 : _mA));
            colorEnable = true;
        }
        ImNodes::BeginNode(__mId);
            ImGui::Button("prev");
            ImNodes::BeginInputAttribute(inputId());
            ImNodes::EndInputAttribute();
            ImGui::SameLine();
            ImNodes::BeginOutputAttribute(outputId());
            ImNodes::EndOutputAttribute();
            ImGui::Button("next");

            if (drawData) drawData();
            ImNodes::SetNodeDraggable(__mId, true);

            if (__mUpdatePos && _mX >= 0 && _mY >= 0) {
                ImNodes::SetNodeGridSpacePos(__mId, {_mX, _mY});
            }
        ImNodes::EndNode();
        if (colorEnable) ImNodes::PopColorStyle();
    }
protected: // interface impl
    void _render() override {
        render(nullptr);
    }
private:
    int __mId;
    bool __mUpdatePos;
    bool __mVisible;
};

}

} // object
} // hanim

#endif