
#ifndef __HANIM_OBJECT_DSVISUAL_HPP__HANIM
#define __HANIM_OBJECT_DSVISUAL_HPP__HANIM

#include <Hanim.hpp>

#include <string>

#include <imgui.h>
#include <imnodes.h>

namespace hanim {
namespace object {
namespace dsvisual {

class LNode : public hanim::HObjectTemplate {
    using ID = unsigned long long;
public:
    LNode(ID id) : HObjectTemplate(), __mId { id }, __mTitle {""}, __mUpdatePos { true }, __mVisible { true }  { }
    LNode & operator=(const LNode &node) {
        __mId = node.__mId;
        __mUpdatePos = node.__mUpdatePos;
        __mVisible = node.__mVisible;
        __mTitle = node.__mTitle;
    }
public:
    // -1: <-
    // 0 : <->
    // 1 : ->
    static void connect(const LNode &node1, const LNode &node2, int flag = 0) {
        Link(node2.linkIdL(), node2.inputId(), node1.outputId(), IM_COL32(255, 0, 0, 100));
        Link(node1.linkIdR(), node1.outputId(), node2.inputId(), IM_COL32(0, 255, 0, 150));
        if (flag == 1)
            disconnect(node1, node2, -1);
        else if (flag == -1)
            disconnect(node1, node2, 1);
    }

    static void disconnect(const LNode &node1, const LNode &node2, int flag = 0) {
        if (flag == 1)
            ImNodes::Link(node1.linkIdR(), 0, 0);
        else if (flag == -1)
            ImNodes::Link(node2.linkIdL(), 0, 0);
        else {
            ImNodes::Link(node1.linkIdR(), 0, 0);
            ImNodes::Link(node2.linkIdL(), 0, 0);
        }
    }

    static void Link(const ID id, const ID portId1, const ID portId2, unsigned int color) {
        ImNodes::PushColorStyle(ImNodesCol_Link, color);
        ImNodes::Link(id, portId1, portId2);
        ImNodes::PopColorStyle();
    } 
public:
    ID id() const { return __mId; }
    ID inputId() const { return __mId + 1; }
    ID outputId() const { return __mId + 2; }
    ID linkIdL() const { return __mId + 3; }
    ID linkIdR() const { return __mId + 4; }
public:
    void setId(ID id) { __mId = id; };
    void setUpdatePos(bool update = true) {
        __mUpdatePos = update;
    }
    void setVisible(bool visible) {
        __mVisible = visible;
    }
    void setTitle(std::string title) {
        __mTitle = title;
    }
public:
    void render(std::function<void ()> drawData = nullptr) {

        if (!__mVisible) return;

        bool colorEnable = false;
        if (_mR >= 0 && _mG >= 0 && _mB >= 0) { // TODO: optimize _mA and (r,g,b)
            ImNodes::PushColorStyle(ImNodesCol_NodeBackground, IM_COL32(_mR, _mG, _mB, _mA < 0 ? 255 : _mA));
            colorEnable = true;
        }

        if (__mUpdatePos && _mX >= 0 && _mY >= 0) {
            ImNodes::SetNodeGridSpacePos(__mId, {_mX, _mY});
        }

        ImNodes::SetNodeDraggable(__mId, true);

        ImNodes::BeginNode(__mId);

            if (__mTitle != "") {
                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted(__mTitle.c_str());
                ImNodes::EndNodeTitleBar();
            }

            ImGui::Button("prev");
            ImNodes::BeginInputAttribute(inputId());
            ImNodes::EndInputAttribute();
            ImGui::SameLine();
            ImNodes::BeginOutputAttribute(outputId());
            ImNodes::EndOutputAttribute();
            ImGui::Button("next");

            if (drawData) drawData();

        ImNodes::EndNode();

        if (colorEnable) ImNodes::PopColorStyle();
    }
protected: // interface impl
    void _render() override {
        render(nullptr);
    }
private:
    ID __mId;
    std::string __mTitle;
    bool __mUpdatePos;
    bool __mVisible;
};

} // dsvisual
} // object
} // hanim

#endif