#ifndef __EMBEDDED_LIST_HPP__DSVISUAL
#define __EMBEDDED_LIST_HPP__DSVISUAL

#include <dstruct.hpp>
#include <dsv-core/dsvisual-core.h>

#include <extends/HanimExtends.hpp>
#include <extends/ImguiExtends.hpp>

namespace dsvisual {
namespace ds {

template <typename NodeType>
class EmbeddedList : public Widget {
public:
    EmbeddedList() : _mUpdateListMutex {}, _mAnimNode{0x123}, _mNodeVec() {
        _mName = "EmbeddedList - " + std::to_string((uint64_t)this);

        // DSVisual
        _mDataVisible = false;
        _mFullScreen = true;

        // Hanim
        hanim::object::dsvisual::Node node(0x1234 + ((uint64_t)this % 0x100) * 0x1234);
        node.setPos(50, 200);
        _mNodeVec.push_back(node);
        _mNodePosXOffset = 200;
        _mNodePosY = 200;
    }

public: // DS interface
    static void init(NodeType *list) {
        NodeType::init(list);
    }

    static bool empty(NodeType *list) {
        return NodeType::empty(list);
    }

    static void add(NodeType *prev, NodeType *curr, EmbeddedList *eListPtr, int frameNumbers = 60) {
        eListPtr->add(prev, curr, frameNumbers);
    }

    static void del(NodeType *prev, NodeType *curr, int ms = 500) {
        // TODO: Animate
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        NodeType::del(prev, curr);
    }

    static NodeType * to_node(typename NodeType::LinkType *link) {
        return NodeType::to_node(link);
    }

    static typename NodeType::LinkType * to_link(NodeType *node) {
        return NodeType::to_link(node);
    }
public:
    void add(NodeType *prev, NodeType *curr, unsigned int animFrameNumbers = 0) {
        hanim::object::dsvisual::Node currNode(_mNodeVec.back().id() + 5);
        if (animFrameNumbers > 10) {

            int prevNodeIndex = find(prev);
            int nextNodeIndex = prevNodeIndex + 1;
            int currNodeIndex = nextNodeIndex;
            float currNodePosX = 50 + _mNodePosXOffset * currNodeIndex;

            // hanimate
            printf("hanim: %f\n", _mNodePosY);
            auto animTree = hanim::animate::dsvisual::InsertAnim(_mNodePosY, _mNodePosXOffset, animFrameNumbers);

            // hobject
            auto hObj = hanim::HObject(
                [ & ](int type, const hanim::IAFrame &frame) {
                    // crash imnodes : Assertion `node_idx_depth_order.Size == GImNodes->NodeIdxSubmissionOrder.Size' failed.
                    // currNode.render();
                    switch(type) {
                        case hanim::InterpolationAnim::MOVE:
                            if (frame.data[1] == -1) { // move x: list
                                _updateListPos(nextNodeIndex, frame.data[0]);
                            } else if (frame.data[0] == -1) { // move y
                                printf("%d: %p - %f, %f\n", animTree.getCurrentFrame(), &_mAnimNode, currNodePosX, frame.data[1]);
                                _mAnimNode.setPos(currNodePosX, frame.data[1]);
                                _mAnimNode.setUpdatePos(true);
                            }
                            break;
                        default: // link anim
                            if (nextNodeIndex < _mNodeVec.size()) {
                                hanim::object::dsvisual::Node::connect(_mNodeVec[prevNodeIndex], _mAnimNode);
                                hanim::object::dsvisual::Node::disconnect(_mNodeVec[prevNodeIndex], _mNodeVec[nextNodeIndex]);
                                if (frame.data[0] > 0.5) {
                                    hanim::object::dsvisual::Node::connect(_mAnimNode, _mNodeVec[nextNodeIndex]);
                                }
                            }
                    }
                }
            );

            _setAnimate(animTree, hObj);

        }

        {   // insert and update
            std::lock_guard<std::mutex> _al(_mUpdateListMutex);
            //float nodePosX = 50 + _mNodePosXOffset * _mNodeVec.size();
            _mNodeVec.push_back(currNode);
            NodeType::add(prev, curr);
            _updateListPos();
        }
    }

    int find(NodeType *nPtr) {
        if (nPtr == &_mHeadNode) return 0;
        int ansPos = 1;
        auto linkPtr = _mHeadNode.link.next;
        while (linkPtr != to_link(nPtr) && linkPtr != to_link(&_mHeadNode)) {
            printf("%d\n", ansPos);
            linkPtr = linkPtr->next;
            ansPos++;
        }
        return ansPos;
    }
public:
    NodeType * headNodePtr() {
        return &_mHeadNode;
    }

protected:
    NodeType _mHeadNode;
    std::mutex _mUpdateListMutex;

/* -----------------------------------------DSVisual-------------------------------------------- */

protected: // top-down interface
    virtual void _drawBasicInfoImpl() override {
        ImGui::Text("HeadNode: %p", &_mHeadNode); ImGui::Separator();
        ImGui::Text("Mode: %s", _mDataVisible ? "Data" : "Link"); ImGui::Separator();
    }

    virtual void _drawVisualImpl() override {
        _mNodePosY = ImGui::GetWindowHeight() / 3;
        ImNodes::BeginNodeEditor();
            // Head Node
            ImNodes::PushColorStyle(ImNodesCol_NodeBackground, IM_COL32(255, 0, 0, 100));
            _mNodeVec[0].render();
            ImNodes::PopColorStyle();

            int index = 1;
            auto prev = _mNodeVec[0];
            {
                std::lock_guard<std::mutex> _al(_mUpdateListMutex);
                for (auto it = headNodePtr()->link.next; it != to_link(headNodePtr()); it = it->next, index++) {
                    _mNodeVec[index].render(
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

                    _mNodeVec[index].setUpdatePos(false);

                    if (index > 1)
                        hanim::object::dsvisual::Node::connect(prev, _mNodeVec[index], true);

                    prev = _mNodeVec[index];
                }
            }

            _playAnimate();

            _mAnimNode.render();

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
    void _updateListPos(int startIndex = 1, float offset = 0) {
        for (int i = startIndex < 1 ? 1 : startIndex; i < _mNodeVec.size(); i++) {
            float nodePosX = 50 + _mNodePosXOffset * i + offset;
            _mNodeVec[i].setPos(nodePosX, _mNodePosY);
            _mNodeVec[i].setUpdatePos(true);
        }
    }

protected:
    bool _mDataVisible;
    float _mNodePosXOffset;
    float _mNodePosY;
    hanim::object::dsvisual::Node _mAnimNode;
    dstruct::Vector<hanim::object::dsvisual::Node> _mNodeVec;
};

}
}

#endif