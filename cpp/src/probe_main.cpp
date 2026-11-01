#include "monx/monster_storage.h"
#include "monx/engine_profile.h"
#include "monx/workspace.h"
#include <iostream>
#include <cassert>
#include <filesystem>

int main(int argc, char* argv[]) {
    std::cout << "Running comprehensive C++ MONx verification suite..." << std::endl;

    // 1. Engine profiles test
    assert(monx::ProfileManager::all().size() == 7);
    const auto& tfs = monx::ProfileManager::get("tfs");
    assert(tfs.id == monx::EngineId::Tfs);

    // 2. Round-trip XML parsing test
    monx::MonsterDoc doc;
    bool ok = monx::MonsterStorage::read_xml("src-tauri/fixtures/engines/tfs/monster/probe hound.xml", doc, tfs);
    assert(ok);
    assert(doc.name == "Probe Hound");
    assert(doc.health_max == 180);
    assert(doc.raceid == 9102);
    assert(doc.voices.size() == 1);
    assert(doc.voices[0].sentence == "Grrrr!");
    assert(doc.loot.size() == 1);
    assert(doc.attacks.size() == 2);
    assert(doc.defenses.size() == 1);
    assert(doc.summons.size() == 1);

    // 3. Write & Re-read round-trip fidelity test
    std::string temp_out = "/tmp/probe_hound_test.xml";
    ok = monx::MonsterStorage::write_xml(temp_out, doc, tfs);
    assert(ok);
    monx::MonsterDoc reread;
    ok = monx::MonsterStorage::read_xml(temp_out, reread, tfs);
    assert(ok);
    assert(reread.name == doc.name);
    assert(reread.health_max == doc.health_max);
    assert(reread.raceid == doc.raceid);
    assert(reread.look.type == doc.look.type);

    // 4. Linting tests
    auto lints = monx::MonsterStorage::lint(doc, tfs);
    assert(lints.empty());

    monx::MonsterDoc invalid_doc;
    invalid_doc.name = "";
    invalid_doc.health_max = 0;
    auto bad_lints = monx::MonsterStorage::lint(invalid_doc, tfs);
    assert(bad_lints.size() >= 2);

    // 5. Workspace open & index test
    monx::WorkspacePaths paths;
    paths.monsters = "src-tauri/fixtures/engines/tfs/monster";
    bool ws_ok = monx::Workspace::instance().open(paths, "tfs");
    assert(ws_ok);
    assert(monx::Workspace::instance().monsters().size() >= 2);
    std::cout << "Indexed " << monx::Workspace::instance().monsters().size() << " monsters in TFS fixture." << std::endl;

    std::cout << "All C++ verification checks PASSED successfully!" << std::endl;
    return 0;
}
