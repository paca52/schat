
#ifndef CHATINPUT_HPP
#define CHATINPUT_HPP

#include "cpptui.hpp"

using cpptui::Input;
using cpptui::Event;

class ChatInput : public Input {
public:
    std::function<void()> on_send;
    bool on_event(const Event &event) override;
};

#endif // CHATINPUT_HPP

