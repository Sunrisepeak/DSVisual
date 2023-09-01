

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
            .setFrameNums(50);
        setFrameTrackIndex(3 * subF); // TODO/Bug?: check finished status

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
    Node(int id) : HObjectTemplate(), __mId { id }, __mUpdatePos { true }  { }
    Node & operator=(const Node &node) {
        __mId = node.__mId;
        __mUpdatePos = node.__mUpdatePos;
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
public:
    void render(std::function<void ()> drawData = nullptr) {
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
                ImNodes::SetNodeEditorSpacePos(__mId, {_mX, _mY});
            }
        ImNodes::EndNode();
    }
protected: // interface impl
    void _render() override {
        render(nullptr);
    }
private:
    int __mId;
    bool __mUpdatePos;
};

}

} // object
} // hanim

#endif