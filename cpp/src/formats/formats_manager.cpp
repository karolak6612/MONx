#include "monx/formats_manager.h"
#include "monx/monster_model.h"
#include "monx/spr_reader.h"
#include "monx/dat_reader.h"
#include "monx/otb_reader.h"
#include "monx/palette.h"
#include <pugixml.hpp>
#include <wx/dcmemory.h>
#include <wx/image.h>

namespace monx {

static std::optional<SprFileReader> s_spr;
static std::optional<DatReader> s_dat;
static std::optional<OtbReader> s_otb;

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
    auto otb = OtbReader::load(path);
    if (!otb) return false;
    s_otb = otb;
    out_server_to_client = otb->server_to_client();
    return true;
}

wxBitmap FormatsManager::render_thing(Category cat, uint32_t id, uint32_t cell) {
    if (s_spr && s_dat) {
        const ThingType* thing = s_dat->get_thing(cat, id);
        if (thing && !thing->sprite_ids.empty()) {
            uint32_t sprite_id = thing->sprite_ids[0];
            SpriteData data = s_spr->sprite_data(sprite_id);
            if (!data.is_empty) {
                auto rgba = SprFileReader::decompress_to_rgba(data.compressed_pixels, true);
                wxImage img(32, 32);
                img.InitAlpha();
                for (int y = 0; y < 32; ++y) {
                    for (int x = 0; x < 32; ++x) {
                        size_t idx = (y * 32 + x) * 4;
                        img.SetRGB(x, y, rgba[idx], rgba[idx + 1], rgba[idx + 2]);
                        img.SetAlpha(x, y, rgba[idx + 3]);
                    }
                }
                if (cell != 32) {
                    img.Rescale(cell, cell, wxIMAGE_QUALITY_NEAREST);
                }
                return wxBitmap(img);
            }
        }
    }

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
    uint32_t id = (look.mode == "typeex") ? look.typeex.value_or(0) : look.type.value_or(0);
    auto [hr, hg, hb] = outfit_palette_color(look.head);
    auto [br, bg, bb] = outfit_palette_color(look.body);

    wxBitmap bmp(cell, cell);
    wxMemoryDC dc(bmp);
    dc.SetBackground(wxBrush(wxColour(30, 30, 30)));
    dc.Clear();

    // Render palette-colored body and head
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(wxColour(hr, hg, hb)));
    dc.DrawCircle(cell / 2, cell / 3, cell / 6); // head with palette color

    dc.SetBrush(wxBrush(wxColour(br, bg, bb)));
    dc.DrawRoundedRectangle(cell / 4, cell / 2, cell / 2, cell / 3, 2); // body with palette color

    // Addons indicator
    if (look.addons > 0) {
        dc.SetBrush(*wxYELLOW_BRUSH);
        dc.DrawCircle(cell / 4, cell / 4, 3);
    }

    dc.SetTextForeground(*wxWHITE);
    wxString text = wxString::Format("%u", id);
    wxSize textSize = dc.GetTextExtent(text);
    dc.DrawText(text, (cell - textSize.GetWidth()) / 2, cell - textSize.GetHeight() - 2);

    return bmp;
}

} // namespace monx
