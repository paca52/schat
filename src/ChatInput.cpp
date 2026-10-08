#include "../h/ChatInput.hpp"

bool ChatInput::on_event(const Event &event) {
    if (event.is_enter() && on_send) {
        on_send();
        return true;
    }
    return Input::on_event(event);
}
