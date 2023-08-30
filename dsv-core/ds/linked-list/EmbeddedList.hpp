#ifndef __EMBEDDED_LIST_HPP__DSVISUAL
#define __EMBEDDED_LIST_HPP__DSVISUAL

#include <dstruct.hpp>
#include <dsv-core/dsvisual-core.h>

#include <extends/ImguiExtends.hpp>

namespace dsvisual {
namespace ds {

template <typename NodeType>
class EmbeddedList : public Widget {
public:
    EmbeddedList() {
        _mName = "EmbeddedList - " + std::to_string((uint64_t)this);
        _mNodeIdVec.push_back((uint64_t)this);
        _mDataVisible = false;
        _mFullScreen = true;
        _mNodePosY = 200;
        //init(&headNode);
    }

public: // DS interface
    static void init(NodeType *list, bool animFlag = true) {
        NodeType::init(list);
    }

    static bool empty(NodeType *list, bool animFlag = true) {
        return NodeType::empty(list);
    }

    static void add(NodeType *prev, NodeType *curr, EmbeddedList *eListPtr = nullptr) {
        if (nullptr != eListPtr) {
            // compute insert pos
            int idIndex = 0 + 1 /*head node*/;
            auto linkPtr = to_link(eListPtr->headNodePtr()); // headNode's link
            while (linkPtr != to_link(prev)) {

                assert(linkPtr == to_link(eListPtr->headNodePtr()));

                idIndex++;
                linkPtr = linkPtr->next;
            }

            printf("add: %d\n", idIndex);

            int prevIdIndex = idIndex - 1;
            int newNodeId = eListPtr->_mNodeIdVec.back() + 5;

            // hanimate
            auto animTree = hanim::ComposeAnim();

            auto anim1 = hanim::move(0, -1, 100, -1, 100);
            animTree.addAnim(anim1, 0);
            auto anim2 = hanim::move(-1, 0, -1, eListPtr->_mNodePosY, 100);
            animTree.addAnim(anim2, anim1.getFrameNums());
            auto anim3 = hanim::scale(0, 1, 300);
            animTree.addAnim(anim3, anim1.getFrameNums() + anim2.getFrameNums());

            animTree.setFrameNums(anim1.getFrameNums() + anim2.getFrameNums() + anim3.getFrameNums());

            // hobject
            auto hObj = hanim::HObject(
                [ & ](int type, const hanim::IAFrame &frame) {
                    auto node = DLNode(newNodeId);
                    auto nodeWidth = ImNodes::GetNodeDimensions(newNodeId).x;

                    float newNodePosX = 50 + (nodeWidth + 100) * idIndex;
                    //printf("frame: %f, %f\n", frame.data[0], frame.data[1]);

                    switch(type) {
                        case hanim::InterpolationAnim::MOVE:
                            if (animTree.getCurrentFrame() < anim1.getFrameNums()) {
                                for (int i = idIndex; i < eListPtr->_mNodeIdVec.size(); i++) {
                                    int nodePosX = 50 + (nodeWidth + 100) * i + frame.data[0];
                                    ImNodes::SetNodeEditorSpacePos(eListPtr->_mNodeIdVec[i], {nodePosX, eListPtr->_mNodePosY});
                                }
                            } else if (animTree.getCurrentFrame() <= anim1.getFrameNums() + anim2.getFrameNums()) {
                                ImNodes::SetNodeEditorSpacePos(node.id(), {newNodePosX, frame.data[1]});
                            }
                            break;
                        case hanim::InterpolationAnim::SCALE:
                            ImNodes::Link(node.id() + 3, node.id() + 1, prevIdIndex + 2);
                            if (frame.data[0] > 0.5)
                                ImNodes::Link(prevIdIndex + 4, prevIdIndex + 2, node.id() + 1);
                            break;
                    }

                    if (type == -1) { // anim end
                        // insert
                        eListPtr->_mNodeIdVec.push_back(newNodeId); // only need to push_back
                        NodeType::add(prev, curr);
                        for (int i = idIndex; i < eListPtr->_mNodeIdVec.size(); i++) { // update node pos
                            int nodePosX = 50 + (nodeWidth + 100) * i;
                            ImNodes::SetNodeEditorSpacePos(eListPtr->_mNodeIdVec[i], {nodePosX, eListPtr->_mNodePosY});
                        }
                    }
                }
            );

            eListPtr->_setAnimate(animTree, hObj);

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        } else {
            NodeType::add(prev, curr);
        }
    }

    static void del(NodeType *prev, NodeType *curr, bool animFlag = true) {
        NodeType::del(prev, curr);
    }

    static NodeType * to_node(typename NodeType::LinkType *link, bool animFlag = true) {
        return NodeType::to_node(link);
    }

    static typename NodeType::LinkType * to_link(NodeType *node, bool animFlag = true) {
        return NodeType::to_link(node);
    }
public:
    NodeType * headNodePtr() {
        return &_mHeadNode;
    }

protected:
    NodeType _mHeadNode;

/* -----------------------------------------DSVisual-------------------------------------------- */

protected: // top-down interface
    virtual void _drawBasicInfoImpl() override {
        ImGui::Text("HeadNode: %p", &_mHeadNode); ImGui::Separator();
        ImGui::Text("Mode: %s", _mDataVisible ? "Data" : "Link"); ImGui::Separator();
    }

    virtual void _drawVisualImpl() override {
        _mNodePosY = ImGui::GetWindowHeight() / 3;
        ImNodes::BeginNodeEditor();
        {
            auto prev = DLNode(_mNodeIdVec[0]);
            int index = 1;
            for (auto it = headNodePtr()->link.next; it != to_link(headNodePtr()); it = it->next, index++) {
                auto curr = DLNode(
                    _mNodeIdVec[index], -1, -1,
                    [&] {
                        if (_mDataVisible) {
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.6f, 1.0f));
                            if (ImGui::Button("Data", {0, 100})) {
                                auto &data = to_node(it)->data;
                                bool visible = data.getVisible();
                                data.setVisible(!visible);
                            }
                            ImGui::PopStyleColor();
                        }
                    }
                );
                ImNodes::Link(prev.linkIdR(), prev.outputId(), curr.inputId());
                ImNodes::Link(curr.linkIdL(), curr.inputId(), prev.outputId());
                prev = curr;
            }
        }

        _playAnimate();

        ImNodes::MiniMap();
        ImNodes::EndNodeEditor();
    }

    virtual void _drawControlImpl() override {
        float wWidth = ImGui::GetWindowWidth();
        float spacingW = ImGui::GetStyle().ItemSpacing.x;
        if (ImGui::Button("Data mode", {wWidth / 2 - 2 * spacingW, 0})) {
            _mDataVisible = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Link mode", {wWidth / 2 - 2 * spacingW, 0})) {
            _mDataVisible = false;
        }
    }

    virtual void _drawAboutImpl() override { }

protected:
    bool _mDataVisible;
    float _mNodePosY;
    dstruct::Vector<int> _mNodeIdVec;
};

}
}

#endif