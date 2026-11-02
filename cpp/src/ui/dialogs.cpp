#include "monx/dialogs.h"
#include "monx/loot_simulator.h"
#include "monx/workspace.h"

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
    sizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 10);
    SetSizer(sizer);
    Center();
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
    : wxDialog(parent, wxID_ANY, "Compare Monsters", wxDefaultPosition, wxSize(700, 500)) {
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Side-by-side Monster Comparison"), 0, wxALL, 10);

    wxBoxSizer* list_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxListBox* left_list = new wxListBox(this, wxID_ANY);
    wxListBox* right_list = new wxListBox(this, wxID_ANY);
    list_sizer->Add(left_list, 1, wxEXPAND | wxALL, 5);
    list_sizer->Add(right_list, 1, wxEXPAND | wxALL, 5);
    sizer->Add(list_sizer, 1, wxEXPAND);

    sizer->Add(CreateButtonSizer(wxOK), 0, wxEXPAND | wxALL, 10);
    SetSizer(sizer);
    Center();
}

// --- Loot Simulator Dialog ---
LootSimDialog::LootSimDialog(wxWindow* parent)
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

    sim_btn->Bind(wxEVT_BUTTON, [this, kills_ctrl, results](wxCommandEvent&) {
        LootSimParams params;
        params.kills_per_session = kills_ctrl->GetValue();

        // Get currently active monster loot or sample loot
        std::vector<LootItem> sample_loot = {
            {.id = 2148, .countmax = 50, .chance = 70000, .comment = "gold coin"},
            {.id = 2152, .countmax = 2, .chance = 15000, .comment = "platinum coin"},
            {.id = 2498, .countmax = 1, .chance = 500, .comment = "royal helmet"}
        };

        auto res = LootSimulator::run(sample_loot, params);
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
