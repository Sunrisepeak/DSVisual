#ifndef __KEY_RECORDER_HPP__DSVISUAL
#define __KEY_RECORDER_HPP__DSVISUAL

// std
#include <string>
// dstruct
#include <dstruct.hpp>
// dsvisual
#include <frameworks/dsvisual-core.hpp>

namespace dsvisual {

namespace input {

class KeyRecorder : public dsvisual::Widget {

private:
    struct __KeyData {
        std::string keyName;
        int time;
    };

    using __InputKeyNameCallback = std::string (*)();

public:
    KeyRecorder(__InputKeyNameCallback cb = nullptr) : _mGetKeyName { cb } { }

protected:
    void _drawBasicInfoImpl() override {

    }
    
    void _drawVisualImpl() override {
        __KeyData kd { "NULL", 1 };

        if (_mGetKeyName == nullptr)
            kd.keyName = __getKeyNameDefaultImpl();
        else
            kd.keyName = _mGetKeyName();

        if (!_mKeyHistory.empty() && _mKeyHistory.back().keyName == kd.keyName) {
            kd.time = _mKeyHistory.back().time + 1;
            _mKeyHistory.pop_back();
        }

        _mKeyHistory.push(kd);

        while (_mKeyHistory.size() > 10) _mKeyHistory.pop();
        for (auto keyData : _mKeyHistory) {
            ImGui::Text("%s -> %d", keyData.keyName.c_str(), keyData.time);
        }
    }

    void _drawControlImpl() override {

    }

private:
    std::string __getKeyNameDefaultImpl() {
        std::string keyName = "NULL";
        auto &io = ImGui::GetIO();
        // 检测键盘输入
        for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) {
            if (io.KeysDown[i] && i >= 32 && i <= 126) {
                keyName = glfwGetKeyName(i, 0);
            }
        }
        return keyName;
    }

protected:
    __InputKeyNameCallback _mGetKeyName;
    dstruct::Deque<__KeyData> _mKeyHistory;
};

}

}



#endif