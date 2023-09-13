#ifndef __EMBEDDED_LIST_HPP__DSVISUAL
#define __EMBEDDED_LIST_HPP__DSVISUAL

#include <dstruct.hpp>
#include <dsv-core/dsvisual-core.h>
#include <animate/HanimExtends.hpp>

namespace dsvisual {
namespace ds {

template <typename NodeType>
class EmbeddedList : public Widget, hanim::HObject {
public:
    EmbeddedList() : _mUpdateListMutex{}, _mAnimNode{ 0 }, _mNodeVec{} {

        auto objAd = reinterpret_cast<unsigned long long>(this);

        // DSVisual
        _mName = "EmbeddedList - " + std::to_string(objAd);
        _mDataVisible = false;
        _mFullScreen = true;

        // Hanim
        // config head-node by use _mAnimNode(tmp)
        _mAnimNode.setId(objAd);
        _mAnimNode.setPos(50, 200);
        _mAnimNode.setColor(0, 0, 255);
        _mAnimNode.setAlpha(255);
        _mNodeVec.push_back(_mAnimNode);

        _mNodePosXOffset = 200;
        _mNodePosY = 250;

        // animate
        _mAType = _AnimType::NONE;
        _mPrevNodeIndex = 0;
        _mNextNodeIndex = 0;
        _mTargetNodeIndex = 0;
        _mAnimNode.setId(objAd - 1); // reset
    }

public: // DS interface
    static void init(NodeType *list) {
        NodeType::init(list);
    }

    static bool empty(NodeType *list) {
        return NodeType::empty(list);
    }

    static NodeType * to_node(typename NodeType::LinkType *link) {
        return NodeType::to_node(link);
    }

    static typename NodeType::LinkType * to_link(NodeType *node) {
        return NodeType::to_link(node);
    }
public:
    void add(NodeType *prev, NodeType *curr, int animFrameNumbers = 0) {
        if (animFrameNumbers > 0) {
            _mAType = _AnimType::Insert;
            _updateAnimateData(prev);
            auto animTree = hanim::animate::dsvisual::InsertAnim(_mNodePosY, _mNodePosXOffset, animFrameNumbers);
            _setAnimate(animTree, *this);
        }

        {   // insert and update
            std::lock_guard<std::mutex> _al(_mUpdateListMutex);
            hanim::object::dsvisual::LNode currNode(_mNodeVec.back().id() + 5);
            _mNodeVec.push_back(currNode);
            NodeType::add(prev, curr);
            _updateListPos();
        }
    }

    void del(NodeType *prev, NodeType *curr, int animFrameNumbers = 0) {

        if (animFrameNumbers > 0) {
            _mAType = _AnimType::Delete;
            _updateAnimateData(prev);
            auto anim = hanim::animate::dsvisual::DeleteAnim(100, animFrameNumbers);
            _setAnimate(anim, *this);
        }

        {
            std::lock_guard<std::mutex> _al(_mUpdateListMutex);
            _mNodeVec.pop_back();
            NodeType::del(prev, curr);
            _updateListPos();
        }
    }

    int find(NodeType *nPtr) {
        if (nPtr == &_mHeadNode) return 0;
        int ansPos = 1;
        auto linkPtr = _mHeadNode.link.next;
        while (linkPtr != to_link(nPtr) && linkPtr != to_link(&_mHeadNode)) {
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
        ImGui::Text("DStruct: EmbeddedList"); ImGui::Separator();
        ImGui::Text("LinkType: Double Link"); ImGui::Separator();
        ImGui::Text("HeadNode: %p", &_mHeadNode); ImGui::Separator();
        ImGui::Text("Visual Mode: %s", _mDataVisible ? "Data" : "Link"); ImGui::Separator();
        ImGui::Text("Link Color: ");
        ImGui::SameLine(); ImGui::TextColored({0, 1, 0, 1}, " --> ");
        ImGui::SameLine(); ImGui::TextColored({1, 0, 0, 1}, " <-- ");
        ImGui::SameLine(); ImGui::TextColored({1, 1, 0, 1}, " <-> ");
    }

    virtual void _drawVisualImpl() override {
        _mNodePosY = ImGui::GetWindowHeight() / 3;
        ImNodes::BeginNodeEditor();
            // Head Node
            _mNodeVec[0].render();
            int prevIndex = 0;
            {
                std::lock_guard<std::mutex> _al(_mUpdateListMutex);
                auto it = _mHeadNode.link.next;
                for (int i = 1; i < _mNodeVec.size(); i++, it = it->next) {

                    _mNodeVec[i].render(
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
                    _mNodeVec[i].setUpdatePos(false);
                    hanim::object::dsvisual::LNode::connect(_mNodeVec[prevIndex], _mNodeVec[i]);
                    prevIndex = i;
                }

                assert(it == to_link(&_mHeadNode));
            }

            if (_playAnimate()) {
                _mAnimNode.render();
            }

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

    virtual void _drawAboutImpl() override {
        {
            ImGui::PushID("EmbeddedList");
            char implLinks[256] = "DStruct: "
                "https://github.com/Sunrisepeak"
                "/DStruct/blob/main"
                "/core/ds/linked-list/EmbeddedList.hpp";
            ImGui::PushItemWidth(ImGui::GetWindowSize().x);
            ImGui::InputText("", implLinks, 256, ImGuiInputTextFlags_ReadOnly);
            ImGui::PopItemWidth();
            ImGui::PopID();
        }
    }

/* -----------------------------------------Hanim: animate impl-------------------------------------------- */

protected:

    enum class _AnimType {
        NONE,
        Insert,
        Delete,
    };

    _AnimType _mAType;
    int _mPrevNodeIndex;
    int _mNextNodeIndex;
    int _mTargetNodeIndex;
    hanim::object::dsvisual::LNode _mAnimNode; // avoid render crash, details - dsvisual-issue1

    void _updateAnimateData(NodeType *prev) {
        _mPrevNodeIndex = find(prev);
        if (_mAType == _AnimType::Insert) {
            _mNextNodeIndex = _mPrevNodeIndex + 1;
            _mTargetNodeIndex = _mNextNodeIndex;
            _mAnimNode.setColor(0, 255, 0);
            _mAnimNode.setVisible(true);
        } else if (_mAType == _AnimType::Delete) {
            _mTargetNodeIndex = _mPrevNodeIndex + 1;
            _mNextNodeIndex = _mPrevNodeIndex + 2;
            _mAnimNode.setVisible(false);
            _mNodeVec[_mTargetNodeIndex].setColor(255, 0, 0);
        }
    }

    void _interpolationHAnimate(int type, const hanim::IAFrame &frame) override {
        switch(_mAType) {
            case _AnimType::Insert:
                _insertAnim(type, frame);
                break;
            case _AnimType::Delete:
                _deleteAnim(type, frame);
                break;
            default:
                break;
        }
    }

    void _insertAnim(int type, const hanim::IAFrame &frame) {
        switch(type) {
            case hanim::InterpolationAnim::MOVE:
                if (frame.data[1] == -1) { // move x: list
                    _updateListPos(_mNextNodeIndex, frame.data[0]);
                } else if (frame.data[0] == -1) { // move y
                    float currNodePosX = 50 + _mNodePosXOffset * _mTargetNodeIndex;
                    _mAnimNode.setPos(currNodePosX, frame.data[1]);
                    _mAnimNode.setUpdatePos(true);
                    if (frame.data[1] >= _mNodePosY / 2 + 1) {
                        hanim::object::dsvisual::LNode::connect(_mNodeVec[_mPrevNodeIndex], _mAnimNode, -1);
                        if (_mNextNodeIndex < _mNodeVec.size()) {
                            hanim::object::dsvisual::LNode::connect(_mAnimNode, _mNodeVec[_mNextNodeIndex], 1);
                            hanim::object::dsvisual::LNode::disconnect(_mNodeVec[_mPrevNodeIndex], _mNodeVec[_mNextNodeIndex]);
                        }
                    }
                }
                break;
            case hanim::InterpolationAnim::ALPHA:
                _mAnimNode.setAlpha(frame.data[0]);
                break;
            case hanim::InterpolationAnim::CUSTOM: // link anim
            //case -1: // end-frame
                hanim::object::dsvisual::LNode::connect(_mNodeVec[_mPrevNodeIndex], _mAnimNode, -1);
                if (_mNextNodeIndex < _mNodeVec.size()) {
                    if (frame.data[0] > 0.25 && _mNextNodeIndex < _mNodeVec.size())
                        hanim::object::dsvisual::LNode::connect(_mAnimNode, _mNodeVec[_mNextNodeIndex], 1);
                    if (frame.data[0] > 0.5 && _mNextNodeIndex < _mNodeVec.size()) {
                        hanim::object::dsvisual::LNode::disconnect(_mNodeVec[_mPrevNodeIndex], _mNodeVec[_mNextNodeIndex]);
                    }
                }
                break;
        }
    }

    void _deleteAnim(int type, const hanim::IAFrame &frame) {
        float currNodePosX;
        switch(type) {
            case hanim::InterpolationAnim::MOVE:
                currNodePosX = 50 + _mNodePosXOffset * _mTargetNodeIndex;
                _mNodeVec[_mTargetNodeIndex].setPos(currNodePosX, _mNodePosY + frame.data[1]);
                _mNodeVec[_mTargetNodeIndex].setUpdatePos(true);
                break;
            case hanim::InterpolationAnim::CUSTOM: // link anim
            case -1: // end frame
                if (_mNextNodeIndex < _mNodeVec.size()) {
                    hanim::object::dsvisual::LNode::connect(_mNodeVec[_mPrevNodeIndex], _mNodeVec[_mNextNodeIndex], 1);
                    if (frame.data[0] > 1)
                        hanim::object::dsvisual::LNode::connect(_mNodeVec[_mPrevNodeIndex], _mNodeVec[_mNextNodeIndex], -1);
                    if (frame.data[0] > 2) {
                        hanim::object::dsvisual::LNode::disconnect(_mNodeVec[_mPrevNodeIndex], _mNodeVec[_mTargetNodeIndex], -1);
                        hanim::object::dsvisual::LNode::disconnect(_mNodeVec[_mTargetNodeIndex], _mNodeVec[_mNextNodeIndex], 1);
                    }
                    if (frame.data[0] > 3) {
                        _mNodeVec[_mTargetNodeIndex].setVisible(false);
                        _updateListPos(_mNextNodeIndex, _mNodePosXOffset * (frame.data[0] - 3) * -1);
                    }
                }
                break;
        }
    }

protected:
    void _updateListPos(int startIndex = 0, float offset = 0) {
        for (int i = startIndex < 0 ? 0 : startIndex; i < _mNodeVec.size(); i++) {
            float nodePosX = 50 + _mNodePosXOffset * i + offset;
            _mNodeVec[i].setPos(nodePosX, _mNodePosY);
            _mNodeVec[i].setUpdatePos(true);
            _mNodeVec[i].setVisible(true);
            if (i != 0)
                _mNodeVec[i].setColor(-1, -1, -1); // set default
        }
    }

protected:
    bool _mDataVisible;
    float _mNodePosXOffset;
    float _mNodePosY;
    dstruct::Vector<hanim::object::dsvisual::LNode> _mNodeVec;
};

}
}

#endif