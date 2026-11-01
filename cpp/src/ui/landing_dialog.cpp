#include "monx/landing_dialog.h"
#include <wx/dirdlg.h>

namespace monx {

LandingDialog::LandingDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "MONx — Open Workspace", wxDefaultPosition, wxSize(640, 480)) {

    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* header = new wxStaticText(this, wxID_ANY, "Select Workspace Folders");
    wxFont f = header->GetFont();
    f.SetPointSize(14);
    f.SetWeight(wxFONTWEIGHT_BOLD);
    header->SetFont(f);
    main_sizer->Add(header, 0, wxALL, 15);

    auto add_row = [&](const wxString& label, wxTextCtrl*& ctrl, bool required) {
        wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* lbl = new wxStaticText(this, wxID_ANY, label + (required ? " *" : ""), wxDefaultPosition, wxSize(120, -1));
        ctrl = new wxTextCtrl(this, wxID_ANY);
        wxButton* btn = new wxButton(this, wxID_ANY, "Browse...", wxDefaultPosition, wxSize(80, -1));

        btn->Bind(wxEVT_BUTTON, [this, ctrl, label](wxCommandEvent&) {
            on_browse(ctrl, "Select " + label + " Folder");
        });

        row->Add(lbl, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        row->Add(ctrl, 1, wxEXPAND | wxRIGHT, 5);
        row->Add(btn, 0);
        main_sizer->Add(row, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    };

    add_row("Monsters", m_monsters_ctrl, true);
    add_row("Items", m_items_ctrl, false);
    add_row("Client", m_client_ctrl, false);
    add_row("Spells", m_spells_ctrl, false);

    // Engine Choice
    wxBoxSizer* eng_row = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* eng_lbl = new wxStaticText(this, wxID_ANY, "Server Engine:", wxDefaultPosition, wxSize(120, -1));
    wxArrayString engines;
    engines.Add("Auto-detect");
    for (const auto& p : ProfileManager::all()) {
        engines.Add(wxString::Format("%s (%s)", p.name, p.key));
    }
    m_engine_choice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, engines);
    m_engine_choice->SetSelection(0);

    eng_row->Add(eng_lbl, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    eng_row->Add(m_engine_choice, 1, wxEXPAND);
    main_sizer->Add(eng_row, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 15);

    main_sizer->AddStretchSpacer();

    wxSizer* btn_sizer = CreateButtonSizer(wxOK | wxCANCEL);
    if (btn_sizer) {
        main_sizer->Add(btn_sizer, 0, wxEXPAND | wxALL, 15);
    }

    SetSizer(main_sizer);
    Center();
}

void LandingDialog::on_browse(wxTextCtrl* target, const wxString& title) {
    wxDirDialog dlg(this, title, target->GetValue(), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dlg.ShowModal() == wxID_OK) {
        target->SetValue(dlg.GetPath());
    }
}

WorkspacePaths LandingDialog::get_paths() const {
    WorkspacePaths p;
    p.monsters = m_monsters_ctrl->GetValue().ToStdString();
    p.items = m_items_ctrl->GetValue().ToStdString();
    p.client = m_client_ctrl->GetValue().ToStdString();
    p.spells = m_spells_ctrl->GetValue().ToStdString();
    return p;
}

std::string LandingDialog::get_engine() const {
    int sel = m_engine_choice->GetSelection();
    if (sel <= 0) return "";
    return ProfileManager::all()[sel - 1].key;
}

} // namespace monx
