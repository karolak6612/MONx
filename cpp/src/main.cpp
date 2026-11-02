#include <wx/wx.h>
#include "monx/main_window.h"

namespace monx {

class MonxApp : public wxApp {
public:
    virtual bool OnInit() override {
        wxInitAllImageHandlers();
        MainWindow* win = new MainWindow();
        win->Show(true);

        // Auto-open fixture workspace if available
        WorkspacePaths paths;
        paths.monsters = "../src-tauri/fixtures/engines/tfs/monster";
        if (!wxDirExists(paths.monsters)) {
            paths.monsters = "src-tauri/fixtures/engines/tfs/monster";
        }
        win->load_workspace(paths, "tfs");

        return true;
    }
};

} // namespace monx

#ifndef MONX_PROBE_ONLY
wxIMPLEMENT_APP(monx::MonxApp);
#endif
