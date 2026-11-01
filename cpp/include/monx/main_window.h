#pragma once

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/spinctrl.h>
#include <wx/srchctrl.h>
#include "monx/workspace.h"
#include "monx/monster_storage.h"

namespace monx {

class MainWindow : public wxFrame {
public:
    MainWindow();
    ~MainWindow();

    void open_landing();
    void load_workspace(const WorkspacePaths& paths, const std::string& engine_key);
    void select_monster(const std::string& file);
    void save_current_monster();

private:
    void init_menu();
    void init_ui();

    // Event handlers
    void on_open_workspace(wxCommandEvent& evt);
    void on_save_monster(wxCommandEvent& evt);
    void on_quit(wxCommandEvent& evt);
    void on_monster_selected(wxListEvent& evt);
    void on_search_monsters(wxCommandEvent& evt);

    // Tools / Dialogs
    void on_batch_edit(wxCommandEvent& evt);
    void on_compare(wxCommandEvent& evt);
    void on_loot_sim(wxCommandEvent& evt);
    void on_quick_open(wxCommandEvent& evt);
    void on_preferences(wxCommandEvent& evt);
    void on_hotkeys(wxCommandEvent& evt);
    void on_about(wxCommandEvent& evt);

    wxAuiManager m_mgr;

    // UI elements
    wxSearchCtrl* m_search_ctrl = nullptr;
    wxListView* m_monster_list = nullptr;
    wxImageList* m_image_list = nullptr;

    // Monster Editor Panes
    wxNotebook* m_editor_tabs = nullptr;
    wxTextCtrl* m_name_ctrl = nullptr;
    wxTextCtrl* m_desc_ctrl = nullptr;
    wxSpinCtrl* m_raceid_ctrl = nullptr;
    wxSpinCtrl* m_health_ctrl = nullptr;
    wxSpinCtrl* m_exp_ctrl = nullptr;
    wxSpinCtrl* m_speed_ctrl = nullptr;
    wxSpinCtrl* m_look_type_ctrl = nullptr;
    wxSpinCtrl* m_look_corpse_ctrl = nullptr;

    // Preview Panel
    wxStaticBitmap* m_outfit_preview = nullptr;
    wxStaticText* m_preview_stats = nullptr;

    // Linter Panel
    wxListView* m_lint_list = nullptr;

    std::string m_current_file;
    MonsterDoc m_current_doc;
};

} // namespace monx
