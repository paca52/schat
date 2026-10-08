#ifndef CHATUI_HPP
#define CHATUI_HPP

#include "../h/cpptui.hpp"
#include "../h/ChatInput.hpp"

using namespace cpptui;

class ChatUI {
public:
    void applyColors();
    std::shared_ptr<Border> makeBorder(const std::string &title);

    std::string username = "ja";

    void addMessage(const std::string &user, const std::string &text, Color user_color);

    void send();
    void selectChannel(const std::string &name);
    std::shared_ptr<Widget> buildSidebar();
    std::shared_ptr<Widget> buildChat();
    int run();

private:
    App app;
    std::shared_ptr<TableScrollable> messages;
    std::shared_ptr<ChatInput> input;
    std::shared_ptr<Border> chatBorder;
    std::shared_ptr<StatusBar> status;
    std::shared_ptr<TextList> channels;
    std::string currentChannel = "general";
    std::vector<std::string> channelNames;
    std::vector<std::shared_ptr<Border>> borders;
    std::shared_ptr<TableScrollable> channelTable;
    bool dark = true;
};
#endif // CHATUI_HPP

