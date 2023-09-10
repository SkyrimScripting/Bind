set_languages("c++20")

add_rules("mode.debug", "mode.releasedbg")

add_repositories("MrowrMisc https://github.com/MrowrMisc/PackageTesting.git")

add_requires("skyrim-commonlib")

target("BIND")
    add_files("*.cpp")
    add_packages("skyrim-commonlib")
    add_rules("@skyrim-commonlib/plugin", {
        mod_folders = {
            "C:/Users/mrowr/AppData/Local/ModOrganizer/GOG Skyrim AE/Mods",
            os.getenv("SKYRIM_SCRIPTING_MOD_FOLDERS")
        }
    })
