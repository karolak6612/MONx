#include "monx/formats_manager.h"
#include "monx/monster_model.h"
#include <pugixml.hpp>
#include <wx/dcmemory.h>
#include <wx/dcclient.h>

namespace monx {

bool FormatsManager::load_items_xml(const std::string& path, std::unordered_map<uint32_t, ItemInfo>& out_items) {
    pugi::xml_document doc;
    if (!doc.load_file(path.c_str())) return false;

    pugi::xml_node root = doc.child("items");
    if (!root) return false;

    for (pugi::xml_node item = root.child("item"); item; item = item.next_sibling("item")) {
        uint32_t id = item.attribute("id").as_uint();
        if (id == 0) continue;
        ItemInfo info;
        info.server_id = id;
        info.client_id = id;
        info.name = item.attribute("name").as_string();

        for (pugi::xml_node attr = item.child("attribute"); attr; attr = attr.next_sibling("attribute")) {
            std::string key = attr.attribute("key").as_string();
            if (key == "weight") {
                info.weight = attr.attribute("value").as_uint();
            } else if (key == "containerSize") {
                info.is_container = true;
            }
        }
        out_items[id] = info;
    }
    return true;
}

bool FormatsManager::load_items_otb(const std::string& path, std::unordered_map<uint32_t, uint32_t>& out_server_to_client) {
    // Basic OTB loader stub
    return true;
}

wxBitmap FormatsManager::render_thing(Category cat, uint32_t id, uint32_t cell) {
    wxBitmap bmp(cell, cell);
    wxMemoryDC dc(bmp);
    dc.SetBackground(*wxBLACK_BRUSH);
    dc.Clear();

    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxLIGHT_GREY_BRUSH);
    dc.DrawRoundedRectangle(4, 4, cell - 8, cell - 8, 4);

    dc.SetTextForeground(*wxBLACK);
    wxString text = wxString::Format("%u", id);
    wxSize textSize = dc.GetTextExtent(text);
    dc.DrawText(text, (cell - textSize.GetWidth()) / 2, (cell - textSize.GetHeight()) / 2);

    return bmp;
}

wxBitmap FormatsManager::render_look(const Look& look, uint32_t dir, uint32_t cell) {
    wxBitmap bmp(cell, cell);
    wxMemoryDC dc(bmp);
    dc.SetBackground(wxBrush(wxColour(30, 30, 30)));
    dc.Clear();

    // Render placeholder outfit silhouette
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(wxColour(100, 150, 240)));
    dc.DrawCircle(cell / 2, cell / 3, cell / 6); // head

    dc.SetBrush(wxBrush(wxColour(80, 120, 200)));
    dc.DrawRoundedRectangle(cell / 4, cell / 2, cell / 2, cell / 3, 2); // body

    // Text with type
    dc.SetTextForeground(*wxWHITE);
    uint32_t id = (look.mode == "typeex") ? look.typeex.value_or(0) : look.type.value_or(0);
    wxString text = wxString::Format("%u", id);
    wxSize textSize = dc.GetTextExtent(text);
    dc.DrawText(text, (cell - textSize.GetWidth()) / 2, cell - textSize.GetHeight() - 2);

    return bmp;
}

} // namespace monx
