#pragma once

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "monx/monster_model.h"

namespace monx {

class BatchEditDialog : public wxDialog {
public:
    BatchEditDialog(wxWindow* parent);

    std::string get_field() const;
    double get_scale() const;
    bool is_scale() const;

private:
    wxChoice* m_field_choice = nullptr;
    wxRadioButton* m_set_val_rb = nullptr;
    wxRadioButton* m_scale_rb = nullptr;
    wxSpinCtrlDouble* m_scale_val = nullptr;
};

class CompareDialog : public wxDialog {
public:
    CompareDialog(wxWindow* parent);
};

class LootSimDialog : public wxDialog {
public:
    LootSimDialog(wxWindow* parent, const std::vector<LootItem>& loot = {});
};

class PreferencesDialog : public wxDialog {
public:
    PreferencesDialog(wxWindow* parent);
};

class HotkeysDialog : public wxDialog {
public:
    HotkeysDialog(wxWindow* parent);
};

class CreateWizard : public wxDialog {
public:
    CreateWizard(wxWindow* parent);
    std::string get_name() const;
    std::string get_file() const;

private:
    wxTextCtrl* m_name_ctrl = nullptr;
    wxTextCtrl* m_file_ctrl = nullptr;
};

} // namespace monx
