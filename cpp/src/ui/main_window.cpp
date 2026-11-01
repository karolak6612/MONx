#include "monx/main_window.h"
#include "monx/landing_dialog.h"
#include "monx/dialogs.h"
#include <wx/srchctrl.h>
#include <wx/aboutdlg.h>
#include <wx/msgdlg.h>

namespace monx {

enum {
    ID_OPEN_WORKSPACE = wxID_HIGHEST + 1,
    ID_SAVE_MONSTER,
    ID_BATCH_EDIT,
    ID_COMPARE,
    ID_LOOT_SIM,
    ID_QUICK_OPEN,
    ID_PREFERENCES,
    ID_HOTKEYS,
    ID_MONSTER_LIST,
    ID_SEARCH_CTRL
};

MainWindow::MainWindow()
    : wxFrame(nullptr, wxID_ANY, "MONx — Monster Editor", wxDefaultPosition, wxSize(1280, 800)) {

    m_mgr.SetManagedWindow(this);

    init_menu();
    init_ui();

    m_mgr.Update();
    Center();
}

MainWindow::~MainWindow() {
    m_mgr.UnInit();
}

void MainWindow::init_menu() {
    wxMenuBar* mb = new wxMenuBar();

    // File
    wxMenu* file_menu = new wxMenu();
    file_menu->Append(ID_OPEN_WORKSPACE, "&Open Workspace...\tCtrl+O");
    file_menu->Append(ID_SAVE_MONSTER, "&Save Monster\tCtrl+S");
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, "E&xit\tAlt+F4");
    mb->Append(file_menu, "&File");

    // Tools
    wxMenu* tools_menu = new wxMenu();
    tools_menu->Append(ID_QUICK_OPEN, "&Quick Open...\tCtrl+P");
    tools_menu->Append(ID_BATCH_EDIT, "&Batch Edit...\tCtrl+B");
    tools_menu->Append(ID_COMPARE, "&Compare Monsters...");
    tools_menu->Append(ID_LOOT_SIM, "&Loot Simulator...");
    mb->Append(tools_menu, "&Tools");

    // Preferences
    wxMenu* pref_menu = new wxMenu();
    pref_menu->Append(ID_PREFERENCES, "&Preferences...");
    pref_menu->Append(ID_HOTKEYS, "&Hotkeys...");
    mb->Append(pref_menu, "&Preferences");

    // Help
    wxMenu* help_menu = new wxMenu();
    help_menu->Append(wxID_ABOUT, "&About MONx");
    mb->Append(help_menu, "&Help");

    SetMenuBar(mb);

    Bind(wxEVT_MENU, &MainWindow::on_open_workspace, this, ID_OPEN_WORKSPACE);
    Bind(wxEVT_MENU, &MainWindow::on_save_monster, this, ID_SAVE_MONSTER);
    Bind(wxEVT_MENU, &MainWindow::on_quit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MainWindow::on_quick_open, this, ID_QUICK_OPEN);
    Bind(wxEVT_MENU, &MainWindow::on_batch_edit, this, ID_BATCH_EDIT);
    Bind(wxEVT_MENU, &MainWindow::on_compare, this, ID_COMPARE);
    Bind(wxEVT_MENU, &MainWindow::on_loot_sim, this, ID_LOOT_SIM);
    Bind(wxEVT_MENU, &MainWindow::on_preferences, this, ID_PREFERENCES);
    Bind(wxEVT_MENU, &MainWindow::on_hotkeys, this, ID_HOTKEYS);
    Bind(wxEVT_MENU, &MainWindow::on_about, this, wxID_ABOUT);
}

void MainWindow::init_ui() {
    // Left Pane: Monster List + Search
    wxPanel* left_panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* left_sizer = new wxBoxSizer(wxVERTICAL);

    m_search_ctrl = new wxSearchCtrl(left_panel, ID_SEARCH_CTRL);
    m_search_ctrl->SetDescriptiveText("Filter monsters...");
    m_search_ctrl->ShowSearchButton(true);
    m_search_ctrl->ShowCancelButton(true);
    left_sizer->Add(m_search_ctrl, 0, wxEXPAND | wxALL, 5);

    m_monster_list = new wxListView(left_panel, ID_MONSTER_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    m_monster_list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 160);
    m_monster_list->InsertColumn(1, "Race ID", wxLIST_FORMAT_RIGHT, 60);
    m_monster_list->InsertColumn(2, "Health", wxLIST_FORMAT_RIGHT, 70);
    left_sizer->Add(m_monster_list, 1, wxEXPAND | wxALL, 5);

    left_panel->SetSizer(left_sizer);
    m_mgr.AddPane(left_panel, wxAuiPaneInfo().Left().Caption("Monsters").BestSize(320, -1).MinSize(240, -1).CloseButton(false));

    // Center Pane: Monster Editor (Tabbed)
    m_editor_tabs = new wxNotebook(this, wxID_ANY);

    // Identity Tab
    wxPanel* identity_tab = new wxPanel(m_editor_tabs, wxID_ANY);
    wxFlexGridSizer* id_sizer = new wxFlexGridSizer(2, 5, 10);
    id_sizer->AddGrowableCol(1, 1);

    id_sizer->Add(new wxStaticText(identity_tab, wxID_ANY, "Name:"), 0, wxALIGN_CENTER_VERTICAL);
    m_name_ctrl = new wxTextCtrl(identity_tab, wxID_ANY);
    id_sizer->Add(m_name_ctrl, 1, wxEXPAND);

    id_sizer->Add(new wxStaticText(identity_tab, wxID_ANY, "Description:"), 0, wxALIGN_CENTER_VERTICAL);
    m_desc_ctrl = new wxTextCtrl(identity_tab, wxID_ANY);
    id_sizer->Add(m_desc_ctrl, 1, wxEXPAND);

    id_sizer->Add(new wxStaticText(identity_tab, wxID_ANY, "Race ID:"), 0, wxALIGN_CENTER_VERTICAL);
    m_raceid_ctrl = new wxSpinCtrl(identity_tab, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 999999);
    id_sizer->Add(m_raceid_ctrl, 1, wxEXPAND);

    id_sizer->Add(new wxStaticText(identity_tab, wxID_ANY, "Max Health:"), 0, wxALIGN_CENTER_VERTICAL);
    m_health_ctrl = new wxSpinCtrl(identity_tab, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 9999999);
    id_sizer->Add(m_health_ctrl, 1, wxEXPAND);

    id_sizer->Add(new wxStaticText(identity_tab, wxID_ANY, "Experience:"), 0, wxALIGN_CENTER_VERTICAL);
    m_exp_ctrl = new wxSpinCtrl(identity_tab, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 9999999);
    id_sizer->Add(m_exp_ctrl, 1, wxEXPAND);

    id_sizer->Add(new wxStaticText(identity_tab, wxID_ANY, "Speed:"), 0, wxALIGN_CENTER_VERTICAL);
    m_speed_ctrl = new wxSpinCtrl(identity_tab, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 9999);
    id_sizer->Add(m_speed_ctrl, 1, wxEXPAND);

    identity_tab->SetSizer(id_sizer);
    m_editor_tabs->AddPage(identity_tab, "Identity");

    // Look Tab
    wxPanel* look_tab = new wxPanel(m_editor_tabs, wxID_ANY);
    wxFlexGridSizer* look_sizer = new wxFlexGridSizer(2, 5, 10);
    look_sizer->AddGrowableCol(1, 1);

    look_sizer->Add(new wxStaticText(look_tab, wxID_ANY, "Look Type / ID:"), 0, wxALIGN_CENTER_VERTICAL);
    m_look_type_ctrl = new wxSpinCtrl(look_tab, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 999999);
    look_sizer->Add(m_look_type_ctrl, 1, wxEXPAND);

    look_sizer->Add(new wxStaticText(look_tab, wxID_ANY, "Corpse ID:"), 0, wxALIGN_CENTER_VERTICAL);
    m_look_corpse_ctrl = new wxSpinCtrl(look_tab, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 999999);
    look_sizer->Add(m_look_corpse_ctrl, 1, wxEXPAND);

    look_tab->SetSizer(look_sizer);
    m_editor_tabs->AddPage(look_tab, "Look & Outfit");

    // Other Tabs (Combat, Loot, Spells)
    m_editor_tabs->AddPage(new wxPanel(m_editor_tabs, wxID_ANY), "Combat");
    m_editor_tabs->AddPage(new wxPanel(m_editor_tabs, wxID_ANY), "Loot");
    m_editor_tabs->AddPage(new wxPanel(m_editor_tabs, wxID_ANY), "Spells");
    m_editor_tabs->AddPage(new wxPanel(m_editor_tabs, wxID_ANY), "Voices");

    m_mgr.AddPane(m_editor_tabs, wxAuiPaneInfo().CenterPane());

    // Right Pane: Preview Panel
    wxPanel* right_panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* right_sizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* prev_header = new wxStaticText(right_panel, wxID_ANY, "Preview & Stats");
    wxFont f = prev_header->GetFont();
    f.SetWeight(wxFONTWEIGHT_BOLD);
    prev_header->SetFont(f);
    right_sizer->Add(prev_header, 0, wxALL, 10);

    m_outfit_preview = new wxStaticBitmap(right_panel, wxID_ANY, FormatsManager::render_thing(Category::Outfit, 0, 96));
    right_sizer->Add(m_outfit_preview, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 10);

    m_preview_stats = new wxStaticText(right_panel, wxID_ANY, "No monster selected");
    right_sizer->Add(m_preview_stats, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);

    right_panel->SetSizer(right_sizer);
    m_mgr.AddPane(right_panel, wxAuiPaneInfo().Right().Caption("Preview").BestSize(260, -1).MinSize(200, -1));

    // Bottom Pane: Linter Panel
    wxPanel* bottom_panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* bot_sizer = new wxBoxSizer(wxVERTICAL);

    m_lint_list = new wxListView(bottom_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    m_lint_list->InsertColumn(0, "Severity", wxLIST_FORMAT_LEFT, 80);
    m_lint_list->InsertColumn(1, "Code", wxLIST_FORMAT_LEFT, 150);
    m_lint_list->InsertColumn(2, "Field", wxLIST_FORMAT_LEFT, 100);
    m_lint_list->InsertColumn(3, "Message", wxLIST_FORMAT_LEFT, 450);
    bot_sizer->Add(m_lint_list, 1, wxEXPAND);

    bottom_panel->SetSizer(bot_sizer);
    m_mgr.AddPane(bottom_panel, wxAuiPaneInfo().Bottom().Caption("Linter / Problems").BestSize(-1, 150).MinSize(-1, 100));

    // Bind list events
    m_monster_list->Bind(wxEVT_LIST_ITEM_SELECTED, &MainWindow::on_monster_selected, this);
    m_search_ctrl->Bind(wxEVT_TEXT, &MainWindow::on_search_monsters, this);
}

void MainWindow::open_landing() {
    LandingDialog dlg(this);
    if (dlg.ShowModal() == wxID_OK) {
        WorkspacePaths paths = dlg.get_paths();
        std::string engine = dlg.get_engine();
        load_workspace(paths, engine);
    }
}

void MainWindow::load_workspace(const WorkspacePaths& paths, const std::string& engine_key) {
    if (Workspace::instance().open(paths, engine_key)) {
        m_monster_list->DeleteAllItems();
        const auto& summaries = Workspace::instance().monsters();
        for (size_t i = 0; i < summaries.size(); ++i) {
            const auto& s = summaries[i];
            long idx = m_monster_list->InsertItem(i, s.name);
            m_monster_list->SetItem(idx, 1, wxString::Format("%u", s.raceid));
            m_monster_list->SetItem(idx, 2, wxString::Format("%u", s.health));
            m_monster_list->SetItemData(idx, (wxIntPtr)i);
        }

        wxString title = "MONx — " + wxString(Workspace::instance().profile().name) + " (" + wxString::Format("%zu", summaries.size()) + " monsters)";
        SetTitle(title);
        if (!summaries.empty()) {
            m_monster_list->Select(0);
        }
    } else {
        wxMessageBox("Failed to open monsters folder!", "Error", wxICON_ERROR | wxOK, this);
    }
}

void MainWindow::select_monster(const std::string& file) {
    auto doc_opt = Workspace::instance().get_monster(file);
    if (!doc_opt.has_value()) return;

    m_current_file = file;
    m_current_doc = *doc_opt;

    m_name_ctrl->SetValue(m_current_doc.name);
    m_desc_ctrl->SetValue(m_current_doc.name_description);
    m_raceid_ctrl->SetValue(m_current_doc.raceid);
    m_health_ctrl->SetValue(m_current_doc.health_max);
    m_exp_ctrl->SetValue(m_current_doc.experience);
    m_speed_ctrl->SetValue(m_current_doc.speed);

    uint32_t look_id = (m_current_doc.look.mode == "typeex") ? m_current_doc.look.typeex.value_or(0) : m_current_doc.look.type.value_or(0);
    m_look_type_ctrl->SetValue(look_id);
    m_look_corpse_ctrl->SetValue(m_current_doc.look.corpse);

    // Update preview
    m_outfit_preview->SetBitmap(FormatsManager::render_look(m_current_doc.look, 2, 96));
    m_preview_stats->SetLabel(wxString::Format(
        "File: %s\nHealth: %u\nExp: %u\nSpeed: %u\nVoices: %zu\nLoot items: %zu\nAttacks: %zu",
        file, m_current_doc.health_max, m_current_doc.experience, m_current_doc.speed,
        m_current_doc.voices.size(), m_current_doc.loot.size(), m_current_doc.attacks.size()
    ));

    // Lints
    m_lint_list->DeleteAllItems();
    auto lints = MonsterStorage::lint(m_current_doc, Workspace::instance().profile());
    for (size_t i = 0; i < lints.size(); ++i) {
        long idx = m_lint_list->InsertItem(i, lints[i].severity);
        m_lint_list->SetItem(idx, 1, lints[i].code);
        m_lint_list->SetItem(idx, 2, lints[i].field);
        m_lint_list->SetItem(idx, 3, lints[i].message);
    }
}

void MainWindow::save_current_monster() {
    if (m_current_file.empty()) return;

    m_current_doc.name = m_name_ctrl->GetValue().ToStdString();
    m_current_doc.name_description = m_desc_ctrl->GetValue().ToStdString();
    m_current_doc.raceid = m_raceid_ctrl->GetValue();
    m_current_doc.health_max = m_health_ctrl->GetValue();
    m_current_doc.health_now = m_health_ctrl->GetValue();
    m_current_doc.experience = m_exp_ctrl->GetValue();
    m_current_doc.speed = m_speed_ctrl->GetValue();

    if (m_current_doc.look.mode == "typeex") {
        m_current_doc.look.typeex = m_look_type_ctrl->GetValue();
    } else {
        m_current_doc.look.type = m_look_type_ctrl->GetValue();
    }
    m_current_doc.look.corpse = m_look_corpse_ctrl->GetValue();

    if (Workspace::instance().save_monster(m_current_doc)) {
        wxMessageBox("Saved successfully!", "MONx", wxICON_INFORMATION | wxOK, this);
    } else {
        wxMessageBox("Failed to save monster file!", "Error", wxICON_ERROR | wxOK, this);
    }
}

void MainWindow::on_open_workspace(wxCommandEvent&) {
    open_landing();
}

void MainWindow::on_save_monster(wxCommandEvent&) {
    save_current_monster();
}

void MainWindow::on_quit(wxCommandEvent&) {
    Close(true);
}

void MainWindow::on_monster_selected(wxListEvent& evt) {
    long item = evt.GetIndex();
    const auto& summaries = Workspace::instance().monsters();
    if (item >= 0 && item < (long)summaries.size()) {
        select_monster(summaries[item].file);
    }
}

void MainWindow::on_search_monsters(wxCommandEvent&) {
    wxString term = m_search_ctrl->GetValue().Lower();
    m_monster_list->DeleteAllItems();
    const auto& summaries = Workspace::instance().monsters();
    for (size_t i = 0; i < summaries.size(); ++i) {
        const auto& s = summaries[i];
        if (term.empty() || wxString(s.name).Lower().Contains(term) || wxString(s.file).Lower().Contains(term)) {
            long idx = m_monster_list->InsertItem(m_monster_list->GetItemCount(), s.name);
            m_monster_list->SetItem(idx, 1, wxString::Format("%u", s.raceid));
            m_monster_list->SetItem(idx, 2, wxString::Format("%u", s.health));
        }
    }
}

void MainWindow::on_batch_edit(wxCommandEvent&) {
    BatchEditDialog dlg(this);
    dlg.ShowModal();
}

void MainWindow::on_compare(wxCommandEvent&) {
    CompareDialog dlg(this);
    dlg.ShowModal();
}

void MainWindow::on_loot_sim(wxCommandEvent&) {
    LootSimDialog dlg(this);
    dlg.ShowModal();
}

void MainWindow::on_quick_open(wxCommandEvent&) {
    wxTextEntryDialog dlg(this, "Quick Jump to Monster (fuzzy search):", "Quick Open");
    if (dlg.ShowModal() == wxID_OK) {
        wxString query = dlg.GetValue().Lower();
        const auto& summaries = Workspace::instance().monsters();
        for (const auto& s : summaries) {
            if (wxString(s.name).Lower().Contains(query)) {
                select_monster(s.file);
                break;
            }
        }
    }
}

void MainWindow::on_preferences(wxCommandEvent&) {
    wxMessageBox("Preferences: configure tab visibility, linter rules, and default engine options.", "Preferences", wxOK | wxICON_INFORMATION, this);
}

void MainWindow::on_hotkeys(wxCommandEvent&) {
    wxMessageBox("Hotkeys: keyboard shortcuts editor.", "Hotkeys", wxOK | wxICON_INFORMATION, this);
}

void MainWindow::on_about(wxCommandEvent&) {
    wxAboutDialogInfo info;
    info.SetName("MONx");
    info.SetVersion("0.1.133");
    info.SetDescription("Native C++ / wxWidgets rewrite of MONx OpenTibia monster editor.\n\nAll 7 engine profiles supported.");
    info.SetCopyright("(C) 2025 Coldensjo");
    wxAboutBox(info);
}

} // namespace monx
