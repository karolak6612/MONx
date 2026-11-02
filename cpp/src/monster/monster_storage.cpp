#include "monx/monster_storage.h"
#include "monx/lua_monster_parser.h"
#include <pugixml.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

namespace monx {

bool MonsterStorage::read_xml(const std::string& path, MonsterDoc& out_doc, const EngineProfile& profile) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.c_str(), pugi::parse_default | pugi::parse_comments | pugi::parse_declaration);
    if (!result) return false;

    // Preserve raw content
    std::ifstream f(path);
    if (f) {
        std::stringstream ss;
        ss << f.rdbuf();
        out_doc.raw_content = ss.str();
    }

    pugi::xml_node root = doc.child("monster");
    if (!root) return false;

    out_doc.name = root.attribute("name").as_string();
    out_doc.name_description = root.attribute("nameDescription").as_string();
    if (out_doc.name_description.empty()) {
        out_doc.name_description = root.attribute("namedescription").as_string();
    }

    out_doc.raceid = root.attribute(profile.raceid_attr.c_str()).as_uint();
    out_doc.experience = root.attribute("experience").as_uint();
    out_doc.speed = root.attribute("speed").as_uint(200);
    out_doc.manacost = root.attribute("manacost").as_uint();

    pugi::xml_node health = root.child("health");
    if (health) {
        out_doc.health_now = health.attribute("now").as_uint(100);
        out_doc.health_max = health.attribute("max").as_uint(100);
    }

    pugi::xml_node look = root.child("look");
    if (look) {
        if (look.attribute("typeex")) {
            out_doc.look.mode = "typeex";
            out_doc.look.typeex = look.attribute("typeex").as_uint();
        } else {
            out_doc.look.mode = "type";
            if (look.attribute("type")) {
                out_doc.look.type = look.attribute("type").as_uint();
            }
        }
        out_doc.look.head = look.attribute("head").as_uint();
        out_doc.look.body = look.attribute("body").as_uint();
        out_doc.look.legs = look.attribute("legs").as_uint();
        out_doc.look.feet = look.attribute("feet").as_uint();
        out_doc.look.addons = look.attribute("addons").as_uint();
        out_doc.look.mount = look.attribute("mount").as_uint();
        out_doc.look.corpse = look.attribute("corpse").as_uint();
        out_doc.look.corpseactionid = look.attribute("corpseactionid").as_uint();
    }

    // Flags
    pugi::xml_node flags = root.child("flags");
    if (flags) {
        for (pugi::xml_node flag = flags.child("flag"); flag; flag = flag.next_sibling("flag")) {
            for (pugi::xml_attribute attr = flag.first_attribute(); attr; attr = attr.next_attribute()) {
                out_doc.flags[attr.name()] = attr.as_string();
            }
        }
    }

    // Voices
    pugi::xml_node voices = root.child("voices");
    if (voices) {
        for (pugi::xml_node voice = voices.child("voice"); voice; voice = voice.next_sibling("voice")) {
            Voice v;
            v.sentence = voice.attribute("sentence").as_string();
            v.yell = voice.attribute("yell").as_uint();
            out_doc.voices.push_back(v);
        }
    }

    // Loot
    pugi::xml_node loot = root.child("loot");
    if (loot) {
        for (pugi::xml_node item = loot.child("item"); item; item = item.next_sibling("item")) {
            LootItem li;
            li.id = item.attribute("id").as_uint();
            li.countmax = item.attribute("countmax").as_uint(1);
            li.chance = item.attribute("chance").as_uint(1000);
            li.action_id = item.attribute("actionId").as_uint();
            out_doc.loot.push_back(li);
        }
    }

    // Attacks
    pugi::xml_node attacks = root.child("attacks");
    if (attacks) {
        for (pugi::xml_node attack = attacks.child("attack"); attack; attack = attack.next_sibling("attack")) {
            Spell s;
            s.name = attack.attribute("name").as_string();
            s.interval = attack.attribute("interval").as_uint(2000);
            s.chance = attack.attribute("chance").as_uint(100);
            if (attack.attribute("min")) s.min = attack.attribute("min").as_int();
            if (attack.attribute("max")) s.max = attack.attribute("max").as_int();
            s.range = attack.attribute("range").as_uint(1);
            s.radius = attack.attribute("radius").as_uint(0);
            s.target = attack.attribute("target").as_uint(0);
            out_doc.attacks.push_back(s);
        }
    }

    // Defenses
    pugi::xml_node defenses = root.child("defenses");
    if (defenses) {
        for (pugi::xml_node def = defenses.child("defense"); def; def = def.next_sibling("defense")) {
            Spell s;
            s.name = def.attribute("name").as_string();
            s.interval = def.attribute("interval").as_uint(2000);
            s.chance = def.attribute("chance").as_uint(100);
            if (def.attribute("min")) s.min = def.attribute("min").as_int();
            if (def.attribute("max")) s.max = def.attribute("max").as_int();
            out_doc.defenses.push_back(s);
        }
    }

    // Summons
    pugi::xml_node summons = root.child("summons");
    if (summons) {
        for (pugi::xml_node summon = summons.child("summon"); summon; summon = summon.next_sibling("summon")) {
            Summon su;
            su.name = summon.attribute("name").as_string();
            su.interval = summon.attribute("interval").as_uint(2000);
            su.chance = summon.attribute("chance").as_uint(100);
            su.max = summon.attribute("max").as_uint(1);
            su.force = summon.attribute("force").as_bool();
            out_doc.summons.push_back(su);
        }
    }

    return true;
}

bool MonsterStorage::write_xml(const std::string& path, const MonsterDoc& doc, const EngineProfile& profile) {
    pugi::xml_document xml;
    pugi::xml_parse_result result = xml.load_file(path.c_str(), pugi::parse_default | pugi::parse_comments | pugi::parse_declaration);
    if (!result) {
        // If file doesn't exist yet, create a new document
        pugi::xml_node decl = xml.prepend_child(pugi::node_declaration);
        decl.append_attribute("version") = "1.0";
        decl.append_attribute("encoding") = "UTF-8";
    }

    pugi::xml_node root = xml.child("monster");
    if (!root) {
        root = xml.append_child("monster");
    }

    root.attribute("name") ? root.attribute("name").set_value(doc.name.c_str()) : root.append_attribute("name") = doc.name.c_str();
    if (!doc.name_description.empty()) {
        const char* nd_attr = (profile.id == EngineId::Tfs) ? "nameDescription" : "nameDescription";
        root.attribute(nd_attr) ? root.attribute(nd_attr).set_value(doc.name_description.c_str()) : root.append_attribute(nd_attr) = doc.name_description.c_str();
    }
    if (doc.raceid > 0) {
        root.attribute(profile.raceid_attr.c_str()) ? root.attribute(profile.raceid_attr.c_str()).set_value(doc.raceid) : root.append_attribute(profile.raceid_attr.c_str()) = doc.raceid;
    }
    root.attribute("experience") ? root.attribute("experience").set_value(doc.experience) : root.append_attribute("experience") = doc.experience;
    root.attribute("speed") ? root.attribute("speed").set_value(doc.speed) : root.append_attribute("speed") = doc.speed;
    root.attribute("manacost") ? root.attribute("manacost").set_value(doc.manacost) : root.append_attribute("manacost") = doc.manacost;

    pugi::xml_node health = root.child("health");
    if (!health) health = root.append_child("health");
    health.attribute("now") ? health.attribute("now").set_value(doc.health_now) : health.append_attribute("now") = doc.health_now;
    health.attribute("max") ? health.attribute("max").set_value(doc.health_max) : health.append_attribute("max") = doc.health_max;

    pugi::xml_node look = root.child("look");
    if (!look) look = root.append_child("look");
    if (doc.look.mode == "typeex" && doc.look.typeex) {
        look.remove_attribute("type");
        look.attribute("typeex") ? look.attribute("typeex").set_value(*doc.look.typeex) : look.append_attribute("typeex") = *doc.look.typeex;
    } else {
        look.remove_attribute("typeex");
        look.attribute("type") ? look.attribute("type").set_value(doc.look.type.value_or(0)) : look.append_attribute("type") = doc.look.type.value_or(0);
        look.attribute("head") ? look.attribute("head").set_value(doc.look.head) : look.append_attribute("head") = doc.look.head;
        look.attribute("body") ? look.attribute("body").set_value(doc.look.body) : look.append_attribute("body") = doc.look.body;
        look.attribute("legs") ? look.attribute("legs").set_value(doc.look.legs) : look.append_attribute("legs") = doc.look.legs;
        look.attribute("feet") ? look.attribute("feet").set_value(doc.look.feet) : look.append_attribute("feet") = doc.look.feet;
        look.attribute("addons") ? look.attribute("addons").set_value(doc.look.addons) : look.append_attribute("addons") = doc.look.addons;
        look.attribute("mount") ? look.attribute("mount").set_value(doc.look.mount) : look.append_attribute("mount") = doc.look.mount;
    }
    if (doc.look.corpse > 0) {
        look.attribute("corpse") ? look.attribute("corpse").set_value(doc.look.corpse) : look.append_attribute("corpse") = doc.look.corpse;
    }

    // Flags
    if (!doc.flags.empty()) {
        pugi::xml_node flags = root.child("flags");
        if (!flags) flags = root.append_child("flags");
        flags.remove_children();
        for (const auto& [k, v] : doc.flags) {
            pugi::xml_node flag = flags.append_child("flag");
            flag.append_attribute(k.c_str()) = v.c_str();
        }
    }

    // Attacks
    if (!doc.attacks.empty()) {
        pugi::xml_node attacks = root.child("attacks");
        if (!attacks) attacks = root.append_child("attacks");
        attacks.remove_children();
        for (const auto& atk : doc.attacks) {
            pugi::xml_node node = attacks.append_child("attack");
            node.append_attribute("name") = atk.name.c_str();
            node.append_attribute("interval") = atk.interval;
            node.append_attribute("chance") = atk.chance;
            if (atk.min) node.append_attribute("min") = *atk.min;
            if (atk.max) node.append_attribute("max") = *atk.max;
            if (atk.range > 1) node.append_attribute("range") = atk.range;
            if (atk.radius > 0) node.append_attribute("radius") = atk.radius;
            if (atk.target > 0) node.append_attribute("target") = atk.target;
        }
    }

    // Defenses
    if (!doc.defenses.empty()) {
        pugi::xml_node defenses = root.child("defenses");
        if (!defenses) defenses = root.append_child("defenses");
        defenses.remove_children();
        for (const auto& def : doc.defenses) {
            pugi::xml_node node = defenses.append_child("defense");
            node.append_attribute("name") = def.name.c_str();
            node.append_attribute("interval") = def.interval;
            node.append_attribute("chance") = def.chance;
            if (def.min) node.append_attribute("min") = *def.min;
            if (def.max) node.append_attribute("max") = *def.max;
        }
    }

    // Elements / Resistances
    if (!doc.elements.empty()) {
        pugi::xml_node elements = root.child("elements");
        if (!elements) elements = root.append_child("elements");
        elements.remove_children();
        for (const auto& [elem, val] : doc.elements) {
            if (val != 0) {
                pugi::xml_node el = elements.append_child("element");
                std::string attr = elem + "Percent";
                el.append_attribute(attr.c_str()) = val;
            }
        }
    }

    // Immunities
    if (!doc.immunities.empty()) {
        pugi::xml_node immunities = root.child("immunities");
        if (!immunities) immunities = root.append_child("immunities");
        immunities.remove_children();
        for (const auto& [imm, val] : doc.immunities) {
            if (val) {
                pugi::xml_node im = immunities.append_child("immunity");
                im.append_attribute(imm.c_str()) = "1";
            }
        }
    }

    // Voices
    if (!doc.voices.empty()) {
        pugi::xml_node voices = root.child("voices");
        if (!voices) voices = root.append_child("voices");
        voices.remove_children();
        for (const auto& v : doc.voices) {
            pugi::xml_node node = voices.append_child("voice");
            node.append_attribute("sentence") = v.sentence.c_str();
            if (v.yell > 0) node.append_attribute("yell") = v.yell;
        }
    }

    // Loot
    if (!doc.loot.empty()) {
        pugi::xml_node loot = root.child("loot");
        if (!loot) loot = root.append_child("loot");
        loot.remove_children();
        for (const auto& li : doc.loot) {
            pugi::xml_node node = loot.append_child("item");
            node.append_attribute("id") = li.id;
            if (li.countmax > 1) node.append_attribute("countmax") = li.countmax;
            node.append_attribute("chance") = li.chance;
            if (li.action_id > 0) node.append_attribute("actionId") = li.action_id;
        }
    }

    // Summons
    if (!doc.summons.empty()) {
        pugi::xml_node summons = root.child("summons");
        if (!summons) summons = root.append_child("summons");
        summons.remove_children();
        for (const auto& su : doc.summons) {
            pugi::xml_node node = summons.append_child("summon");
            node.append_attribute("name") = su.name.c_str();
            node.append_attribute("interval") = su.interval;
            node.append_attribute("chance") = su.chance;
            node.append_attribute("max") = su.max;
            if (su.force) node.append_attribute("force") = "1";
        }
    }

    return xml.save_file(path.c_str(), "\t", pugi::format_default | pugi::format_write_bom);
}

bool MonsterStorage::read_lua(const std::string& path, MonsterDoc& out_doc, const EngineProfile& profile) {
    std::ifstream f(path);
    if (!f) return false;
    std::stringstream ss;
    ss << f.rdbuf();
    return LuaMonsterParser::parse(ss.str(), out_doc);
}

bool MonsterStorage::write_lua(const std::string& path, const MonsterDoc& doc, const EngineProfile& profile) {
    std::ofstream f(path);
    if (!f) return false;
    f << LuaMonsterParser::serialize(doc);
    return true;
}

std::vector<Lint> MonsterStorage::lint(const MonsterDoc& doc, const EngineProfile& profile) {
    std::vector<Lint> lints;

    if (doc.name.empty()) {
        lints.push_back({doc.file, "name.empty", "error", "Monster name cannot be empty", "name"});
    }
    if (doc.health_max == 0) {
        lints.push_back({doc.file, "health.zero", "error", "Max health must be greater than zero", "health"});
    }
    if (doc.health_now > doc.health_max) {
        lints.push_back({doc.file, "health.now-exceeds-max", "warning", "Initial health exceeds max health", "health"});
    }
    if (doc.look.mode == "type" && !doc.look.type.has_value()) {
        lints.push_back({doc.file, "look.missing-type", "warning", "Look has no type specified", "look"});
    }
    return lints;
}

} // namespace monx
