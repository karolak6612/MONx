#include "monx/main_window.h"
#include "monx/landing_dialog.h"
#include "monx/dialogs.h"
#include <wx/srchctrl.h>
#include <wx/aboutdlg.h>
#include <wx/msgdlg.h>
#include <wx/numdlg.h>
#include <wx/textdlg.h>

namespace monx {

enum {
    ID_NEW_MONSTER = wxID_HIGHEST + 1,
    ID_OPEN_WORKSPACE,
    ID_SAVE_MONSTER,
    ID_BATCH_EDIT,
    ID_COMPARE,
    ID_LOOT_SIM,
    ID_QUICK_OPEN,
    ID_PREFERENCES,
    ID_HOTKEYS,
    ID_MONSTER_LIST,
    ID_SEARCH_CTRL,
    ID_ADD_LOOT,
    ID_REMOVE_LOOT,
    ID_ADD_ATTACK,
    ID_ADD_DEFENSE,
    ID_REMOVE_SPELL,
    ID_ADD_VOICE,
    ID_REMOVE_VOICE
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
    file_menu->Append(ID_NEW_MONSTER, "&New Monster...\tCtrl+N");
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

    Bind(wxEVT_MENU, &MainWindow::on_new_monster, this, ID_NEW_MONSTER);
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

    // 1. Identity Tab
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

    id_sizer->Add(new wxStaticText(identity_tab, wxID_ANY, "Mana Cost:"), 0, wxALIGN_CENTER_VERTICAL);
    m_manacost_ctrl = new wxSpinCtrl(identity_tab, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 99999);
    id_sizer->Add(m_manacost_ctrl, 1, wxEXPAND);

    identity_tab->SetSizer(id_sizer);
    m_editor_tabs->AddPage(identity_tab, "Identity");

    // 2. Look Tab
    wxPanel* look_tab = new wxPanel(m_editor_tabs, wxID_ANY);
    wxFlexGridSizer* look_sizer = new wxFlexGridSizer(2, 5, 10);
    look_sizer->AddGrowableCol(1, 1);

    look_sizer->Add(new wxStaticText(look_tab, wxID_ANY, "Look Type / ID:"), 0, wxALIGN_CENTER_VERTICAL);
    m_look_type_ctrl = new wxSpinCtrl(look_tab, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 999999);
    look_sizer->Add(m_look_type_ctrl, 1, wxEXPAND);

    look_sizer->Add(new wxStaticText(look_tab, wxID_ANY, "Head:"), 0, wxALIGN_CENTER_VERTICAL);
    m_look_head_ctrl = new wxSpinCtrl(look_tab, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 132);
    look_sizer->Add(m_look_head_ctrl, 1, wxEXPAND);

    look_sizer->Add(new wxStaticText(look_tab, wxID_ANY, "Body:"), 0, wxALIGN_CENTER_VERTICAL);
    m_look_body_ctrl = new wxSpinCtrl(look_tab, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 132);
    look_sizer->Add(m_look_body_ctrl, 1, wxEXPAND);

    look_sizer->Add(new wxStaticText(look_tab, wxID_ANY, "Legs:"), 0, wxALIGN_CENTER_VERTICAL);
    m_look_legs_ctrl = new wxSpinCtrl(look_tab, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 132);
    look_sizer->Add(m_look_legs_ctrl, 1, wxEXPAND);

    look_sizer->Add(new wxStaticText(look_tab, wxID_ANY, "Feet:"), 0, wxALIGN_CENTER_VERTICAL);
    m_look_feet_ctrl = new wxSpinCtrl(look_tab, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 132);
    look_sizer->Add(m_look_feet_ctrl, 1, wxEXPAND);

    look_sizer->Add(new wxStaticText(look_tab, wxID_ANY, "Addons:"), 0, wxALIGN_CENTER_VERTICAL);
    m_look_addons_ctrl = new wxSpinCtrl(look_tab, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 3);
    look_sizer->Add(m_look_addons_ctrl, 1, wxEXPAND);

    look_sizer->Add(new wxStaticText(look_tab, wxID_ANY, "Corpse ID:"), 0, wxALIGN_CENTER_VERTICAL);
    m_look_corpse_ctrl = new wxSpinCtrl(look_tab, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 999999);
    look_sizer->Add(m_look_corpse_ctrl, 1, wxEXPAND);

    look_tab->SetSizer(look_sizer);
    m_editor_tabs->AddPage(look_tab, "Look & Outfit");

    // 3. Combat Tab
    wxPanel* combat_tab = new wxPanel(m_editor_tabs, wxID_ANY);
    wxFlexGridSizer* combat_sizer = new wxFlexGridSizer(2, 5, 10);
    combat_sizer->AddGrowableCol(1, 1);

    combat_sizer->Add(new wxStaticText(combat_tab, wxID_ANY, "Armor:"), 0, wxALIGN_CENTER_VERTICAL);
    m_armor_ctrl = new wxSpinCtrl(combat_tab, wxID_ANY, "10", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 9999);
    combat_sizer->Add(m_armor_ctrl, 1, wxEXPAND);

    combat_sizer->Add(new wxStaticText(combat_tab, wxID_ANY, "Defense:"), 0, wxALIGN_CENTER_VERTICAL);
    m_defense_ctrl = new wxSpinCtrl(combat_tab, wxID_ANY, "10", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 9999);
    combat_sizer->Add(m_defense_ctrl, 1, wxEXPAND);

    combat_sizer->Add(new wxStaticText(combat_tab, wxID_ANY, "Target Distance:"), 0, wxALIGN_CENTER_VERTICAL);
    m_target_distance_ctrl = new wxSpinCtrl(combat_tab, wxID_ANY, "1", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 10);
    combat_sizer->Add(m_target_distance_ctrl, 1, wxEXPAND);

    combat_sizer->Add(new wxStaticText(combat_tab, wxID_ANY, "Static Attack %:"), 0, wxALIGN_CENTER_VERTICAL);
    m_static_attack_ctrl = new wxSpinCtrl(combat_tab, wxID_ANY, "70", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100);
    combat_sizer->Add(m_static_attack_ctrl, 1, wxEXPAND);

    combat_sizer->Add(new wxStaticText(combat_tab, wxID_ANY, "Run on Health:"), 0, wxALIGN_CENTER_VERTICAL);
    m_run_on_health_ctrl = new wxSpinCtrl(combat_tab, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10000);
    combat_sizer->Add(m_run_on_health_ctrl, 1, wxEXPAND);

    combat_tab->SetSizer(combat_sizer);
    m_editor_tabs->AddPage(combat_tab, "Combat");

    // 4. Resistances Tab
    wxPanel* res_tab = new wxPanel(m_editor_tabs, wxID_ANY);
    wxFlexGridSizer* res_sizer = new wxFlexGridSizer(2, 5, 10);
    res_sizer->AddGrowableCol(1, 1);

    res_sizer->Add(new wxStaticText(res_tab, wxID_ANY, "Physical %:"), 0, wxALIGN_CENTER_VERTICAL);
    m_res_physical_ctrl = new wxSpinCtrl(res_tab, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -100, 100);
    res_sizer->Add(m_res_physical_ctrl, 1, wxEXPAND);

    res_sizer->Add(new wxStaticText(res_tab, wxID_ANY, "Fire %:"), 0, wxALIGN_CENTER_VERTICAL);
    m_res_fire_ctrl = new wxSpinCtrl(res_tab, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -100, 100);
    res_sizer->Add(m_res_fire_ctrl, 1, wxEXPAND);

    res_sizer->Add(new wxStaticText(res_tab, wxID_ANY, "Earth %:"), 0, wxALIGN_CENTER_VERTICAL);
    m_res_earth_ctrl = new wxSpinCtrl(res_tab, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -100, 100);
    res_sizer->Add(m_res_earth_ctrl, 1, wxEXPAND);

    res_sizer->Add(new wxStaticText(res_tab, wxID_ANY, "Energy %:"), 0, wxALIGN_CENTER_VERTICAL);
    m_res_energy_ctrl = new wxSpinCtrl(res_tab, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -100, 100);
    res_sizer->Add(m_res_energy_ctrl, 1, wxEXPAND);

    res_sizer->Add(new wxStaticText(res_tab, wxID_ANY, "Ice %:"), 0, wxALIGN_CENTER_VERTICAL);
    m_res_ice_ctrl = new wxSpinCtrl(res_tab, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -100, 100);
    res_sizer->Add(m_res_ice_ctrl, 1, wxEXPAND);

    res_sizer->Add(new wxStaticText(res_tab, wxID_ANY, "Holy %:"), 0, wxALIGN_CENTER_VERTICAL);
    m_res_holy_ctrl = new wxSpinCtrl(res_tab, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -100, 100);
    res_sizer->Add(m_res_holy_ctrl, 1, wxEXPAND);

    res_sizer->Add(new wxStaticText(res_tab, wxID_ANY, "Death %:"), 0, wxALIGN_CENTER_VERTICAL);
    m_res_death_ctrl = new wxSpinCtrl(res_tab, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -100, 100);
    res_sizer->Add(m_res_death_ctrl, 1, wxEXPAND);

    res_tab->SetSizer(res_sizer);
    m_editor_tabs->AddPage(res_tab, "Resistances");

    // 5. Loot Tab
    wxPanel* loot_tab = new wxPanel(m_editor_tabs, wxID_ANY);
    wxBoxSizer* loot_sizer = new wxBoxSizer(wxVERTICAL);
    m_loot_list = new wxListView(loot_tab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    m_loot_list->InsertColumn(0, "Item ID", wxLIST_FORMAT_LEFT, 80);
    m_loot_list->InsertColumn(1, "Name", wxLIST_FORMAT_LEFT, 150);
    m_loot_list->InsertColumn(2, "Count Max", wxLIST_FORMAT_RIGHT, 80);
    m_loot_list->InsertColumn(3, "Chance (ppm)", wxLIST_FORMAT_RIGHT, 100);
    loot_sizer->Add(m_loot_list, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* loot_btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* add_loot_btn = new wxButton(loot_tab, ID_ADD_LOOT, "Add Drop");
    wxButton* rem_loot_btn = new wxButton(loot_tab, ID_REMOVE_LOOT, "Remove Drop");
    loot_btn_sizer->Add(add_loot_btn, 0, wxRIGHT, 5);
    loot_btn_sizer->Add(rem_loot_btn, 0);
    loot_sizer->Add(loot_btn_sizer, 0, wxALL, 5);

    add_loot_btn->Bind(wxEVT_BUTTON, &MainWindow::on_add_loot, this);
    rem_loot_btn->Bind(wxEVT_BUTTON, &MainWindow::on_remove_loot, this);

    loot_tab->SetSizer(loot_sizer);
    m_editor_tabs->AddPage(loot_tab, "Loot");

    // 6. Spells Tab
    wxPanel* spells_tab = new wxPanel(m_editor_tabs, wxID_ANY);
    wxBoxSizer* spells_sizer = new wxBoxSizer(wxVERTICAL);
    m_spells_list = new wxListView(spells_tab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    m_spells_list->InsertColumn(0, "Spell Name", wxLIST_FORMAT_LEFT, 160);
    m_spells_list->InsertColumn(1, "Interval (ms)", wxLIST_FORMAT_RIGHT, 100);
    m_spells_list->InsertColumn(2, "Chance %", wxLIST_FORMAT_RIGHT, 80);
    m_spells_list->InsertColumn(3, "Min / Max", wxLIST_FORMAT_LEFT, 120);
    spells_sizer->Add(m_spells_list, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* spell_btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* add_atk_btn = new wxButton(spells_tab, ID_ADD_ATTACK, "Add Attack");
    wxButton* add_def_btn = new wxButton(spells_tab, ID_ADD_DEFENSE, "Add Defense");
    wxButton* rem_spell_btn = new wxButton(spells_tab, ID_REMOVE_SPELL, "Remove Spell");
    spell_btn_sizer->Add(add_atk_btn, 0, wxRIGHT, 5);
    spell_btn_sizer->Add(add_def_btn, 0, wxRIGHT, 5);
    spell_btn_sizer->Add(rem_spell_btn, 0);
    spells_sizer->Add(spell_btn_sizer, 0, wxALL, 5);

    add_atk_btn->Bind(wxEVT_BUTTON, &MainWindow::on_add_attack, this);
    add_def_btn->Bind(wxEVT_BUTTON, &MainWindow::on_add_defense, this);
    rem_spell_btn->Bind(wxEVT_BUTTON, &MainWindow::on_remove_spell, this);

    spells_tab->SetSizer(spells_sizer);
    m_editor_tabs->AddPage(spells_tab, "Spells");

    // 7. Voices Tab
    wxPanel* voices_tab = new wxPanel(m_editor_tabs, wxID_ANY);
    wxBoxSizer* voices_sizer = new wxBoxSizer(wxVERTICAL);
    m_voices_list = new wxListView(voices_tab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    m_voices_list->InsertColumn(0, "Sentence", wxLIST_FORMAT_LEFT, 350);
    m_voices_list->InsertColumn(1, "Yell", wxLIST_FORMAT_CENTER, 60);
    voices_sizer->Add(m_voices_list, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* voice_btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* add_voice_btn = new wxButton(voices_tab, ID_ADD_VOICE, "Add Voice");
    wxButton* rem_voice_btn = new wxButton(voices_tab, ID_REMOVE_VOICE, "Remove Voice");
    voice_btn_sizer->Add(add_voice_btn, 0, wxRIGHT, 5);
    voice_btn_sizer->Add(rem_voice_btn, 0);
    voices_sizer->Add(voice_btn_sizer, 0, wxALL, 5);

    add_voice_btn->Bind(wxEVT_BUTTON, &MainWindow::on_add_voice, this);
    rem_voice_btn->Bind(wxEVT_BUTTON, &MainWindow::on_remove_voice, this);

    voices_tab->SetSizer(voices_sizer);
    m_editor_tabs->AddPage(voices_tab, "Voices");

    // 8. Bestiary Tab
    wxPanel* bestiary_tab = new wxPanel(m_editor_tabs, wxID_ANY);
    wxFlexGridSizer* bestiary_sizer = new wxFlexGridSizer(2, 5, 10);
    bestiary_sizer->AddGrowableCol(1, 1);
    bestiary_sizer->Add(new wxStaticText(bestiary_tab, wxID_ANY, "Bestiary Class:"), 0, wxALIGN_CENTER_VERTICAL);
    bestiary_sizer->Add(new wxTextCtrl(bestiary_tab, wxID_ANY, "Amphibic"), 1, wxEXPAND);
    bestiary_sizer->Add(new wxStaticText(bestiary_tab, wxID_ANY, "Difficulty:"), 0, wxALIGN_CENTER_VERTICAL);
    bestiary_sizer->Add(new wxTextCtrl(bestiary_tab, wxID_ANY, "Easy"), 1, wxEXPAND);
    bestiary_sizer->Add(new wxStaticText(bestiary_tab, wxID_ANY, "Occurrence:"), 0, wxALIGN_CENTER_VERTICAL);
    bestiary_sizer->Add(new wxTextCtrl(bestiary_tab, wxID_ANY, "Common"), 1, wxEXPAND);
    bestiary_tab->SetSizer(bestiary_sizer);
    m_editor_tabs->AddPage(bestiary_tab, "Bestiary");

    // 9. Target Strategy Tab
    wxPanel* target_tab = new wxPanel(m_editor_tabs, wxID_ANY);
    wxFlexGridSizer* target_sizer = new wxFlexGridSizer(2, 5, 10);
    target_sizer->AddGrowableCol(1, 1);
    target_sizer->Add(new wxStaticText(target_tab, wxID_ANY, "Nearest Target %:"), 0, wxALIGN_CENTER_VERTICAL);
    target_sizer->Add(new wxSpinCtrl(target_tab, wxID_ANY, "100", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100), 1, wxEXPAND);
    target_sizer->Add(new wxStaticText(target_tab, wxID_ANY, "Lowest Health %:"), 0, wxALIGN_CENTER_VERTICAL);
    target_sizer->Add(new wxSpinCtrl(target_tab, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100), 1, wxEXPAND);
    target_tab->SetSizer(target_sizer);
    m_editor_tabs->AddPage(target_tab, "Target Strategy");

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

    // 1. Identity
    m_name_ctrl->SetValue(m_current_doc.name);
    m_desc_ctrl->SetValue(m_current_doc.name_description);
    m_raceid_ctrl->SetValue(m_current_doc.raceid);
    m_health_ctrl->SetValue(m_current_doc.health_max);
    m_exp_ctrl->SetValue(m_current_doc.experience);
    m_speed_ctrl->SetValue(m_current_doc.speed);
    m_manacost_ctrl->SetValue(m_current_doc.manacost);

    // 2. Look
    uint32_t look_id = (m_current_doc.look.mode == "typeex") ? m_current_doc.look.typeex.value_or(0) : m_current_doc.look.type.value_or(0);
    m_look_type_ctrl->SetValue(look_id);
    m_look_head_ctrl->SetValue(m_current_doc.look.head);
    m_look_body_ctrl->SetValue(m_current_doc.look.body);
    m_look_legs_ctrl->SetValue(m_current_doc.look.legs);
    m_look_feet_ctrl->SetValue(m_current_doc.look.feet);
    m_look_addons_ctrl->SetValue(m_current_doc.look.addons);
    m_look_corpse_ctrl->SetValue(m_current_doc.look.corpse);

    // 3. Combat
    m_target_distance_ctrl->SetValue(std::max(1u, (uint32_t)std::stoul(m_current_doc.flags.count("targetdistance") ? m_current_doc.flags["targetdistance"] : "1")));
    m_static_attack_ctrl->SetValue((uint32_t)std::stoul(m_current_doc.flags.count("staticattack") ? m_current_doc.flags["staticattack"] : "70"));
    m_run_on_health_ctrl->SetValue((uint32_t)std::stoul(m_current_doc.flags.count("runonhealth") ? m_current_doc.flags["runonhealth"] : "0"));

    // 4. Resistances
    m_res_physical_ctrl->SetValue(m_current_doc.elements.count("physical") ? m_current_doc.elements["physical"] : 0);
    m_res_fire_ctrl->SetValue(m_current_doc.elements.count("fire") ? m_current_doc.elements["fire"] : 0);
    m_res_earth_ctrl->SetValue(m_current_doc.elements.count("earth") ? m_current_doc.elements["earth"] : 0);
    m_res_energy_ctrl->SetValue(m_current_doc.elements.count("energy") ? m_current_doc.elements["energy"] : 0);
    m_res_ice_ctrl->SetValue(m_current_doc.elements.count("ice") ? m_current_doc.elements["ice"] : 0);
    m_res_holy_ctrl->SetValue(m_current_doc.elements.count("holy") ? m_current_doc.elements["holy"] : 0);
    m_res_death_ctrl->SetValue(m_current_doc.elements.count("death") ? m_current_doc.elements["death"] : 0);

    // 5. Populate Loot
    m_loot_list->DeleteAllItems();
    for (size_t i = 0; i < m_current_doc.loot.size(); ++i) {
        const auto& item = m_current_doc.loot[i];
        long idx = m_loot_list->InsertItem(i, wxString::Format("%u", item.id));
        std::string iname = item.comment.empty() ? ("Item #" + std::to_string(item.id)) : item.comment;
        m_loot_list->SetItem(idx, 1, iname);
        m_loot_list->SetItem(idx, 2, wxString::Format("%u", item.countmax));
        m_loot_list->SetItem(idx, 3, wxString::Format("%u", item.chance));
    }

    // 6. Populate Spells
    m_spells_list->DeleteAllItems();
    size_t sidx = 0;
    for (const auto& sp : m_current_doc.attacks) {
        long idx = m_spells_list->InsertItem(sidx++, "[Atk] " + sp.name);
        m_spells_list->SetItem(idx, 1, wxString::Format("%u", sp.interval));
        m_spells_list->SetItem(idx, 2, wxString::Format("%u", sp.chance));
        m_spells_list->SetItem(idx, 3, wxString::Format("%d / %d", sp.min.value_or(0), sp.max.value_or(0)));
    }
    for (const auto& sp : m_current_doc.defenses) {
        long idx = m_spells_list->InsertItem(sidx++, "[Def] " + sp.name);
        m_spells_list->SetItem(idx, 1, wxString::Format("%u", sp.interval));
        m_spells_list->SetItem(idx, 2, wxString::Format("%u", sp.chance));
        m_spells_list->SetItem(idx, 3, wxString::Format("%d / %d", sp.min.value_or(0), sp.max.value_or(0)));
    }

    // 7. Populate Voices
    m_voices_list->DeleteAllItems();
    for (size_t i = 0; i < m_current_doc.voices.size(); ++i) {
        const auto& v = m_current_doc.voices[i];
        long idx = m_voices_list->InsertItem(i, v.sentence);
        m_voices_list->SetItem(idx, 1, v.yell ? "Yes" : "No");
    }

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

    // 1. Identity
    m_current_doc.name = m_name_ctrl->GetValue().ToStdString();
    m_current_doc.name_description = m_desc_ctrl->GetValue().ToStdString();
    m_current_doc.raceid = m_raceid_ctrl->GetValue();
    m_current_doc.health_max = m_health_ctrl->GetValue();
    m_current_doc.health_now = m_health_ctrl->GetValue();
    m_current_doc.experience = m_exp_ctrl->GetValue();
    m_current_doc.speed = m_speed_ctrl->GetValue();
    m_current_doc.manacost = m_manacost_ctrl->GetValue();

    // 2. Look
    if (m_current_doc.look.mode == "typeex") {
        m_current_doc.look.typeex = m_look_type_ctrl->GetValue();
    } else {
        m_current_doc.look.type = m_look_type_ctrl->GetValue();
    }
    m_current_doc.look.head = m_look_head_ctrl->GetValue();
    m_current_doc.look.body = m_look_body_ctrl->GetValue();
    m_current_doc.look.legs = m_look_legs_ctrl->GetValue();
    m_current_doc.look.feet = m_look_feet_ctrl->GetValue();
    m_current_doc.look.addons = m_look_addons_ctrl->GetValue();
    m_current_doc.look.corpse = m_look_corpse_ctrl->GetValue();

    // 3. Combat
    m_current_doc.flags["targetdistance"] = std::to_string(m_target_distance_ctrl->GetValue());
    m_current_doc.flags["staticattack"] = std::to_string(m_static_attack_ctrl->GetValue());
    m_current_doc.flags["runonhealth"] = std::to_string(m_run_on_health_ctrl->GetValue());

    // 4. Resistances
    m_current_doc.elements["physical"] = m_res_physical_ctrl->GetValue();
    m_current_doc.elements["fire"] = m_res_fire_ctrl->GetValue();
    m_current_doc.elements["earth"] = m_res_earth_ctrl->GetValue();
    m_current_doc.elements["energy"] = m_res_energy_ctrl->GetValue();
    m_current_doc.elements["ice"] = m_res_ice_ctrl->GetValue();
    m_current_doc.elements["holy"] = m_res_holy_ctrl->GetValue();
    m_current_doc.elements["death"] = m_res_death_ctrl->GetValue();

    if (Workspace::instance().save_monster(m_current_doc)) {
        wxMessageBox("Saved successfully!", "MONx", wxICON_INFORMATION | wxOK, this);
    } else {
        wxMessageBox("Failed to save monster file!", "Error", wxICON_ERROR | wxOK, this);
    }
}

// Handlers for Add / Remove
void MainWindow::on_add_loot(wxCommandEvent&) {
    long id = wxGetNumberFromUser("Enter item ID to drop:", "Item ID:", "Add Loot", 2148, 1, 99999, this);
    if (id > 0) {
        LootItem li;
        li.id = (uint32_t)id;
        li.countmax = 1;
        li.chance = 10000;
        m_current_doc.loot.push_back(li);
        select_monster(m_current_file);
    }
}

void MainWindow::on_remove_loot(wxCommandEvent&) {
    long sel = m_loot_list->GetFirstSelected();
    if (sel >= 0 && sel < (long)m_current_doc.loot.size()) {
        m_current_doc.loot.erase(m_current_doc.loot.begin() + sel);
        select_monster(m_current_file);
    }
}

void MainWindow::on_add_attack(wxCommandEvent&) {
    wxString name = wxGetTextFromUser("Enter spell name (e.g. melee, fire, energy):", "Add Attack Spell", "melee", this);
    if (!name.empty()) {
        Spell sp;
        sp.name = name.ToStdString();
        sp.interval = 2000;
        sp.chance = 100;
        sp.min = -10;
        sp.max = -50;
        m_current_doc.attacks.push_back(sp);
        select_monster(m_current_file);
    }
}

void MainWindow::on_add_defense(wxCommandEvent&) {
    wxString name = wxGetTextFromUser("Enter defense spell name (e.g. healing, speed):", "Add Defense Spell", "healing", this);
    if (!name.empty()) {
        Spell sp;
        sp.name = name.ToStdString();
        sp.interval = 2000;
        sp.chance = 20;
        sp.min = 20;
        sp.max = 50;
        m_current_doc.defenses.push_back(sp);
        select_monster(m_current_file);
    }
}

void MainWindow::on_remove_spell(wxCommandEvent&) {
    long sel = m_spells_list->GetFirstSelected();
    if (sel >= 0) {
        if (sel < (long)m_current_doc.attacks.size()) {
            m_current_doc.attacks.erase(m_current_doc.attacks.begin() + sel);
        } else {
            long dsel = sel - m_current_doc.attacks.size();
            if (dsel < (long)m_current_doc.defenses.size()) {
                m_current_doc.defenses.erase(m_current_doc.defenses.begin() + dsel);
            }
        }
        select_monster(m_current_file);
    }
}

void MainWindow::on_add_voice(wxCommandEvent&) {
    wxString sentence = wxGetTextFromUser("Enter monster voice sentence:", "Add Voice", "Rawr!", this);
    if (!sentence.empty()) {
        Voice v;
        v.sentence = sentence.ToStdString();
        v.yell = 0;
        m_current_doc.voices.push_back(v);
        select_monster(m_current_file);
    }
}

void MainWindow::on_remove_voice(wxCommandEvent&) {
    long sel = m_voices_list->GetFirstSelected();
    if (sel >= 0 && sel < (long)m_current_doc.voices.size()) {
        m_current_doc.voices.erase(m_current_doc.voices.begin() + sel);
        select_monster(m_current_file);
    }
}

void MainWindow::on_new_monster(wxCommandEvent&) {
    CreateWizard wiz(this);
    if (wiz.ShowModal() == wxID_OK) {
        MonsterDoc new_doc;
        new_doc.name = wiz.get_name();
        new_doc.file = wiz.get_file();
        new_doc.health_max = 100;
        new_doc.health_now = 100;
        new_doc.speed = 200;
        Workspace::instance().save_monster(new_doc);
        load_workspace(Workspace::instance().paths(), Workspace::instance().profile().key);
        select_monster(new_doc.file);
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
    LootSimDialog dlg(this, m_current_doc.loot);
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
    PreferencesDialog dlg(this);
    dlg.ShowModal();
}

void MainWindow::on_hotkeys(wxCommandEvent&) {
    HotkeysDialog dlg(this);
    dlg.ShowModal();
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
