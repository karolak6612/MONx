#pragma once

#include <wx/wx.h>
#include <functional>
#include "monx/workspace.h"

namespace monx {

class LandingDialog : public wxDialog {
public:
    LandingDialog(wxWindow* parent);

    WorkspacePaths get_paths() const;
    std::string get_engine() const;

private:
    void on_browse(wxTextCtrl* target, const wxString& title);
    void on_open(wxCommandEvent& evt);

    wxTextCtrl* m_monsters_ctrl = nullptr;
    wxTextCtrl* m_items_ctrl = nullptr;
    wxTextCtrl* m_client_ctrl = nullptr;
    wxTextCtrl* m_spells_ctrl = nullptr;
    wxChoice* m_engine_choice = nullptr;
};

} // namespace monx
