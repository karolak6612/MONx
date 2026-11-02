#include "monx/dialogs.h"
#include "monx/loot_simulator.h"
#include "monx/workspace.h"
#include <wx/listctrl.h>

namespace monx {

// --- Batch Edit Dialog ---
BatchEditDialog::BatchEditDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Batch Edit Corpus", wxDefaultPosition, wxSize(450, 300)) {
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(new wxStaticText(this, wxID_ANY, "Select Field to Modify:"), 0, wxALL, 10);
    wxArrayString fields;
    fields.Add("experience");
    fields.Add("health");
    fields.Add("speed");
    fields.Add("manacost");
    m_field_choice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, fields);
    m_field_choice->SetSelection(0);
    sizer->Add(m_field_choice, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    m_set_val_rb = new wxRadioButton(this, wxID_ANY, "Set to value", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_scale_rb = new wxRadioButton(this, wxID_ANY, "Scale by factor (e.g. 1.25 for +25%)");
    sizer->Add(m_set_val_rb, 0, wxALL, 10);
    sizer->Add(m_scale_rb, 0, wxLEFT | wxRIGHT, 10);

    m_scale_val = new wxSpinCtrlDouble(this, wxID_ANY, "1.0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0.01, 100.0, 1.0, 0.05);
    sizer->Add(m_scale_val, 0, wxALL, 10);

    sizer->AddStretchSpacer();
    wxSizer* btn_sz = CreateButtonSizer(wxOK | wxCANCEL);
    sizer->Add(btn_sz, 0, wxEXPAND | wxALL, 10);
    SetSizer(sizer);
    Center();

    Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) {
        if (evt.GetId() == wxID_OK) {
            std::string field = get_field();
            double factor = get_scale();
            bool scale = is_scale();

            auto& ws = Workspace::instance();
            for (const auto& s : ws.monsters()) {
                auto doc_opt = ws.get_monster(s.file);
                if (doc_opt) {
                    auto doc = *doc_opt;
                    if (field == "experience") {
                        doc.experience = scale ? (uint32_t)(doc.experience * factor) : (uint32_t)factor;
                    } else if (field == "health") {
                        doc.health_max = scale ? (uint32_t)(doc.health_max * factor) : (uint32_t)factor;
                        doc.health_now = doc.health_max;
                    } else if (field == "speed") {
                        doc.speed = scale ? (uint32_t)(doc.speed * factor) : (uint32_t)factor;
                    }
                    ws.save_monster(doc);
                }
            }
            wxMessageBox("Batch edit applied to corpus!", "Batch Edit", wxOK | wxICON_INFORMATION);
            EndModal(wxID_OK);
        } else {
            evt.Skip();
        }
    });
}

std::string BatchEditDialog::get_field() const {
    return m_field_choice->GetStringSelection().ToStdString();
}

double BatchEditDialog::get_scale() const {
    return m_scale_val->GetValue();
}

bool BatchEditDialog::is_scale() const {
    return m_scale_rb->GetValue();
}

// --- Compare Dialog ---
CompareDialog::CompareDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Compare Monsters", wxDefaultPosition, wxSize(750, 500)) {
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Side-by-side Monster Comparison"), 0, wxALL, 10);

    wxBoxSizer* list_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxChoice* left_choice = new wxChoice(this, wxID_ANY);
    wxChoice* right_choice = new wxChoice(this, wxID_ANY);

    const auto& monsters = Workspace::instance().monsters();
    for (const auto& m : monsters) {
        left_choice->Append(m.name, (void*)&m);
        right_choice->Append(m.name, (void*)&m);
    }
    if (!monsters.empty()) {
        left_choice->SetSelection(0);
        right_choice->SetSelection(monsters.size() > 1 ? 1 : 0);
    }

    wxBoxSizer* choices_sizer = new wxBoxSizer(wxHORIZONTAL);
    choices_sizer->Add(left_choice, 1, wxEXPAND | wxALL, 5);
    choices_sizer->Add(right_choice, 1, wxEXPAND | wxALL, 5);
    sizer->Add(choices_sizer, 0, wxEXPAND);

    wxTextCtrl* diff_view = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    sizer->Add(diff_view, 1, wxEXPAND | wxALL, 10);

    auto update_diff = [this, left_choice, right_choice, diff_view]() {
        int l_sel = left_choice->GetSelection();
        int r_sel = right_choice->GetSelection();
        const auto& monsters = Workspace::instance().monsters();
        if (l_sel >= 0 && l_sel < (int)monsters.size() && r_sel >= 0 && r_sel < (int)monsters.size()) {
            auto doc_a = Workspace::instance().get_monster(monsters[l_sel].file);
            auto doc_b = Workspace::instance().get_monster(monsters[r_sel].file);
            if (doc_a && doc_b) {
                wxString text;
                text += wxString::Format("Attribute\t\t\t%s\t\t\t%s\n", doc_a->name, doc_b->name);
                text += "----------------------------------------------------------------------------------------\n";
                text += wxString::Format("Health:\t\t\t%u\t\t\t%u (Delta: %+d)\n", doc_a->health_max, doc_b->health_max, (int)doc_b->health_max - (int)doc_a->health_max);
                text += wxString::Format("Experience:\t\t%u\t\t\t%u (Delta: %+d)\n", doc_a->experience, doc_b->experience, (int)doc_b->experience - (int)doc_a->experience);
                text += wxString::Format("Speed:\t\t\t%u\t\t\t%u (Delta: %+d)\n", doc_a->speed, doc_b->speed, (int)doc_b->speed - (int)doc_a->speed);
                text += wxString::Format("Race ID:\t\t\t%u\t\t\t%u\n", doc_a->raceid, doc_b->raceid);
                text += wxString::Format("Voices:\t\t\t%zu\t\t\t%zu\n", doc_a->voices.size(), doc_b->voices.size());
                text += wxString::Format("Attacks:\t\t\t%zu\t\t\t%zu\n", doc_a->attacks.size(), doc_b->attacks.size());
                text += wxString::Format("Loot Items:\t\t%zu\t\t\t%zu\n", doc_a->loot.size(), doc_b->loot.size());
                diff_view->SetValue(text);
            }
        }
    };

    left_choice->Bind(wxEVT_CHOICE, [update_diff](wxCommandEvent&) { update_diff(); });
    right_choice->Bind(wxEVT_CHOICE, [update_diff](wxCommandEvent&) { update_diff(); });
    update_diff();

    sizer->Add(CreateButtonSizer(wxOK), 0, wxEXPAND | wxALL, 10);
    SetSizer(sizer);
    Center();
}

// --- Loot Simulator Dialog ---
LootSimDialog::LootSimDialog(wxWindow* parent, const std::vector<LootItem>& loot)
    : wxDialog(parent, wxID_ANY, "Loot Simulator", wxDefaultPosition, wxSize(600, 450)) {
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Monte Carlo Loot Simulator"), 0, wxALL, 10);

    wxBoxSizer* opt_sizer = new wxBoxSizer(wxHORIZONTAL);
    opt_sizer->Add(new wxStaticText(this, wxID_ANY, "Kills:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    wxSpinCtrl* kills_ctrl = new wxSpinCtrl(this, wxID_ANY, "1000", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000000);
    opt_sizer->Add(kills_ctrl, 0, wxRIGHT, 15);

    wxButton* sim_btn = new wxButton(this, wxID_ANY, "Simulate Drops");
    opt_sizer->Add(sim_btn, 0);
    sizer->Add(opt_sizer, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);

    wxTextCtrl* results = new wxTextCtrl(this, wxID_ANY, "Click 'Simulate Drops' to run simulation.", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    sizer->Add(results, 1, wxEXPAND | wxALL, 10);

    sim_btn->Bind(wxEVT_BUTTON, [this, kills_ctrl, results, loot](wxCommandEvent&) {
        LootSimParams params;
        params.kills_per_session = kills_ctrl->GetValue();

        std::vector<LootItem> roll_loot = loot;
        if (roll_loot.empty()) {
            roll_loot = {
                {.id = 2148, .countmax = 50, .chance = 70000, .comment = "gold coin"},
                {.id = 2152, .countmax = 2, .chance = 15000, .comment = "platinum coin"},
                {.id = 2498, .countmax = 1, .chance = 500, .comment = "royal helmet"}
            };
        }

        auto res = LootSimulator::run(roll_loot, params);
        wxString report = wxString::Format("Simulated %u kills:\n\n", res.total_kills);
        for (const auto& it : res.items) {
            report += wxString::Format("- %s (ID %u): Total %u dropped (seen in %u kills, %.2f%% drop rate)\n",
                it.name, it.id, it.total_dropped, it.kills_with_drop,
                (double)it.kills_with_drop / (double)res.total_kills * 100.0);
        }
        results->SetValue(report);
    });

    sizer->Add(CreateButtonSizer(wxOK), 0, wxEXPAND | wxALL, 10);
    SetSizer(sizer);
    Center();
}

// --- Preferences Dialog ---
PreferencesDialog::PreferencesDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Preferences", wxDefaultPosition, wxSize(450, 350)) {
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Editor Section Visibility:"), 0, wxALL, 10);

    sizer->Add(new wxCheckBox(this, wxID_ANY, "Show Identity Section"), 0, wxLEFT | wxBOTTOM, 5);
    sizer->Add(new wxCheckBox(this, wxID_ANY, "Show Look & Outfit Section"), 0, wxLEFT | wxBOTTOM, 5);
    sizer->Add(new wxCheckBox(this, wxID_ANY, "Show Combat Section"), 0, wxLEFT | wxBOTTOM, 5);
    sizer->Add(new wxCheckBox(this, wxID_ANY, "Show Resistances Section"), 0, wxLEFT | wxBOTTOM, 5);
    sizer->Add(new wxCheckBox(this, wxID_ANY, "Show Loot Section"), 0, wxLEFT | wxBOTTOM, 5);
    sizer->Add(new wxCheckBox(this, wxID_ANY, "Show Spells Section"), 0, wxLEFT | wxBOTTOM, 5);
    sizer->Add(new wxCheckBox(this, wxID_ANY, "Show Voices Section"), 0, wxLEFT | wxBOTTOM, 5);

    sizer->AddStretchSpacer();
    sizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 10);
    SetSizer(sizer);
    Center();
}

// --- Hotkeys Dialog ---
HotkeysDialog::HotkeysDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Keyboard Shortcuts", wxDefaultPosition, wxSize(500, 380)) {
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Default Key Bindings:"), 0, wxALL, 10);

    wxListView* list = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    list->InsertColumn(0, "Action", wxLIST_FORMAT_LEFT, 250);
    list->InsertColumn(1, "Shortcut", wxLIST_FORMAT_LEFT, 150);

    list->InsertItem(0, "Open Workspace");
    list->SetItem(0, 1, "Ctrl+O");

    list->InsertItem(1, "Save Monster");
    list->SetItem(1, 1, "Ctrl+S");

    list->InsertItem(2, "Quick Open");
    list->SetItem(2, 1, "Ctrl+P");

    list->InsertItem(3, "Batch Edit");
    list->SetItem(3, 1, "Ctrl+B");

    sizer->Add(list, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    sizer->Add(CreateButtonSizer(wxOK), 0, wxEXPAND | wxALL, 10);
    SetSizer(sizer);
    Center();
}

// --- Create Wizard ---
CreateWizard::CreateWizard(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Create New Monster", wxDefaultPosition, wxSize(400, 250)) {
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(new wxStaticText(this, wxID_ANY, "Monster Name:"), 0, wxALL, 10);
    m_name_ctrl = new wxTextCtrl(this, wxID_ANY, "New Monster");
    sizer->Add(m_name_ctrl, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    sizer->Add(new wxStaticText(this, wxID_ANY, "File Name:"), 0, wxALL, 10);
    m_file_ctrl = new wxTextCtrl(this, wxID_ANY, "new_monster.xml");
    sizer->Add(m_file_ctrl, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    sizer->AddStretchSpacer();
    sizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 10);
    SetSizer(sizer);
    Center();
}

std::string CreateWizard::get_name() const {
    return m_name_ctrl->GetValue().ToStdString();
}

std::string CreateWizard::get_file() const {
    return m_file_ctrl->GetValue().ToStdString();
}

} // namespace monx
