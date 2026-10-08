#include "../h/ChatUI.hpp"
#include "../h/Time.hpp"

// Eksplicitne, kontrastne boje (default boja okvira u temi je skoro
// nevidljiva na pozadini). Poziva se i posle svake promene teme.
void ChatUI::applyColors() {
    const Theme &th = Theme::current();
    for (auto &b : borders) b->set_color(th.primary);  // linije + naslov

    Color text = Color::contrast_color(th.input_bg);  // belo/crno po pozadini
    input->fg_color = text;
    input->focused_fg_color = text;
    input->bg_color = th.input_bg;
    input->focused_bg_color = th.selection;
    input->placeholder_color = th.foreground;
    input->cursor_color = th.primary;

    // Izabrani red tabele: čitljiv tekst na istaknutoj pozadini
    for (auto &t : {messages, channelTable}) {
        t->selected_bg_color = th.selection;
        t->selected_fg_color = text;
        t->row_fg_color = th.foreground;
    }
}

std::shared_ptr<Border> ChatUI::makeBorder(const std::string &title) {
    auto b = std::make_shared<Border>(BorderStyle::Rounded);
    if (!title.empty()) b->set_title(title, Alignment::Left);
    borders.push_back(b);
    return b;
}

    void ChatUI::addMessage(const std::string &user, const std::string &text, Color user_color) {
        std::vector<StyledText> row;
        row.emplace_back(StyledText().colored(Time::getTime(), Color(128, 128, 128)));
        row.emplace_back(StyledText().colored_bold(user, user_color));
        row.emplace_back(StyledText(text));
        messages->rows.push_back(std::move(row));
        // Skroluj na poslednju poruku
        messages->selected_index = (int)messages->rows.size() - 1;
        // Pre prvog iscrtavanja visina je 0 pa se ne sme skrolovati
        if (messages->height > 2)
            messages->scroll_to_selection();
        else
            messages->scroll_offset = 0;
    }

void ChatUI::send() {
    std::string text = input->get_value();
    if (text.empty()) return;
    addMessage(username, text, Color::Green());
    input->set_text("");
    input->set_focus(true);  // posle klika na "Pošalji" vrati fokus na unos
                             // TODO: ovde pozovi svoj mrežni sloj (socket / WebSocket / ...)
}

void ChatUI::selectChannel(const std::string &name) {
    currentChannel = name;
    chatBorder->set_title("# " + name, Alignment::Left);
    messages->rows.clear();
    addMessage("sistem", "Ušli ste u #" + name, Color::Yellow());
}

std::shared_ptr<Widget> ChatUI::buildSidebar() {
    auto side = std::make_shared<Vertical>();
    side->min_width = 18;

    // --- Kanali ---
    auto ch_border = makeBorder("Kanali");
    auto ch = std::make_shared<TableScrollable>();
    channelTable = ch;
    ch->columns = {StyledText("")};
    channelNames = {"general", "random", "dev", "offtopic"};
    for (const auto &n : channelNames) ch->add_row({"# " + n});
    ch->on_submit = [this](int i) {
        if (i >= 0 && i < (int)channelNames.size())
            selectChannel(channelNames[i]);
    };
    ch_border->add(ch);
    side->add(ch_border);

    // --- Online korisnici ---
    auto on_border = makeBorder("Online");
    auto users = std::make_shared<TextList>();
    users->style = ListStyle::Bullet;
    users->bullet_markers = {"● "};
    for (auto u : {"ana", "marko", "jelena", "ja"}) users->add_item(u, 0);
    on_border->add(users);
    side->add(on_border);

    return side;
}

std::shared_ptr<Widget> ChatUI::buildChat() {
    auto col = std::make_shared<Vertical>();

    // --- Lista poruka ---
    chatBorder = makeBorder("# general");
    messages = std::make_shared<TableScrollable>();
    messages->columns = {StyledText("Vreme"), StyledText("Korisnik"),
        StyledText("Poruka")};
    messages->col_widths = {6, 10, 0};
    chatBorder->add(messages);
    col->add(chatBorder);

    // --- Unos poruke ---
    auto in_border = makeBorder("");
    in_border->fixed_height = 3;
    auto row = std::make_shared<Horizontal>();
    input = std::make_shared<ChatInput>();
    input->placeholder = "Napiši poruku i pritisni Enter...";
    input->on_send = [this]() { send(); };
    row->add(input);
    auto btn = std::make_shared<Button>("Pošalji", [this]() { send(); });
    btn->fixed_width = 11;
    row->add(btn);
    in_border->add(row);
    col->add(in_border);

    return col;
}

int ChatUI::run() {
    Theme::set_theme(Theme::TokyoNight());
    auto root = std::make_shared<Vertical>();

    // Zaglavlje
    auto header = std::make_shared<Horizontal>();
    header->fixed_height = 1;
    header->add(std::make_shared<Label>(" 💬 Chat"));
    root->add(header);

    // Telo: sidebar | chat (SplitPane, može da se vuče mišem)
    auto split = std::make_shared<SplitPane>();
    split->ratio = 0.25;
    split->set_panes(buildSidebar(), buildChat());
    root->add(split);

    // Prečice
    auto shortcuts = std::make_shared<ShortcutBar>();
    shortcuts->fixed_height = 1;
    shortcuts->add("Enter", StyledText("Pošalji"));
    shortcuts->add("Tab", StyledText("Fokus"));
    shortcuts->add("Ctrl+T", StyledText("Tema"));
    shortcuts->add("Ctrl+Q", StyledText("Izlaz"));
    root->add(shortcuts);

    // Status bar
    status = std::make_shared<StatusBar>();
    status->fixed_height = 1;
    status->add_section(StyledText().colored_bold("● ONLINE", Color::Green()));
    status->add_section(StyledText("korisnik: " + username));
    root->add(status);

    // Globalne prečice
    app.register_key('q', []() { App::quit(); }, /*ctrl=*/true);
    // Enter: biblioteka šalje taster i levoj tabeli kanala čak i kad nema
    // fokus, pa Enter rešavamo ovde, prema tome ko ima fokus.
    auto on_enter = [this]() {
        if (input->has_focus()) {
            send();
        } else if (channelTable->has_focus()) {
            int i = channelTable->selected_index;
            if (i >= 0 && i < (int)channelNames.size())
                selectChannel(channelNames[i]);
        }
    };
    app.register_key(10, on_enter);
    app.register_key(13, on_enter);

    app.register_key('t', [this]() {
            dark = !dark;
            Theme::set_theme(dark ? Theme::TokyoNight() : Theme::Light());
            applyColors();
            }, /*ctrl=*/true);
    applyColors();

    // Demo poruke
    addMessage("sistem", "Dobrodošli u #general", Color::Yellow());
    addMessage("ana", "Zdravo svima!", Color::Cyan());
    addMessage("marko", "Ćao Ana, šta ima novo?", Color::Magenta());

    // Primer: poruka iz pozadinske niti (npr. mrežni sloj)
    //   app.post([&]() { add_message("ana", "...", Color::Cyan()); });

    // App::run() na startu fokusira prvi widget (tabelu kanala), pa fokus na
    // polje za unos postavljamo čim se petlja pokrene.
    app.post([this]() { input->set_focus(true); });

    app.run(root);
    return 0;
}
