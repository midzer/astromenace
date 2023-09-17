/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


    AstroMenace is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AstroMenace is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


    Website: https://viewizard.com/
    Project: https://github.com/viewizard/astromenace
    E-mail: viewizard@viewizard.com

*****************************************************************************/

#ifndef CONFIG_CONFIG_H
#define CONFIG_CONFIG_H

#include "../core/base.h"
#include "../game/weapon_panel.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// since we have POD structures that could be changed in new version,
// make sure we load proper profiles and top scores
// note, xml tag can't contain space characters, use underline instead
constexpr char CONFIG_VERSION[]{"1.4"};
// TODO remove "reserved" in sPilotProfile on CONFIG_VERSION's update
// FIXME WeaponAltControlData in sPilotProfile should be int, look at sGameConfig controls
//       we store SDL_Keycode (int32_t) here

namespace config {
// (!) changes make previous configuration file incompatible

// maximum profiles
constexpr int MAX_PROFILES{5};
// profile name size
constexpr unsigned PROFILE_NAME_SIZE{1024};
// maximum possible missions number for arrays in pilot profile
constexpr unsigned MAX_MISSIONS{100};
// maximum ship's weapon slots
constexpr unsigned MAX_WEAPONS{6};
// maximum hints
constexpr unsigned MAX_HINTS{10};

} // config

namespace config {

constexpr int MAX_SHADOWMAP_SIZE{2048};

} // config

namespace config {
// (!) changes could corrupt 2D rendering

constexpr float VirtualWidth_Wide{1228.0f};
constexpr float VirtualHeight_Wide{768.0f};

constexpr float VirtualWidth_Standard{1024.0f};
constexpr float VirtualHeight_Standard{768.0f};

} // config

// This structure should be POD, since we "pack" it into config file
// as memory block. Don't use std::string or any containers here.
struct sPilotProfile {
    bool Used{false};

    char Name[config::PROFILE_NAME_SIZE];

    // difficulty related settings
    uint8_t EnemyWeaponPenalty{2};          // Enemy weapon fire penalty [1, 3]
    uint8_t EnemyArmorPenalty{2};           // Enemy armor penalty [1, 4]
    uint8_t EnemyTargetingSpeedPenalty{2};  // Enemy targeting speed penalty [1, 4]
    uint8_t UnlimitedAmmo{1};               // 0 - limited ammo, 1 - unlimited ammo
    // note, we use "Limited Ammo" option in menu, but UnlimitedAmmo variable
    uint8_t UndestroyableWeapon{1};         // 0 - destroyable, 1 - undestroyable
    // note, we use "Destroyable Weapon" option, but UndestroyableWeapon variable
    uint8_t WeaponTargetingMode{0};         // 0 - simulator, 1 - arcade
    uint8_t SpaceShipControlMode{1};        // 0 - simulator, 1 - arcade
    uint8_t reserved{0};

    // default player's ship (first ship, without additional hull upgrades)
    uint8_t ShipHull{1};
    uint8_t ShipHullUpgrade{1};
    float ArmorStatus{30.0f};

    // weapon related settings
    uint8_t Weapon[config::MAX_WEAPONS];
    int WeaponAmmo[config::MAX_WEAPONS];
    float WeaponSlotYAngle[config::MAX_WEAPONS];        // weapon Y angle
    uint8_t WeaponControl[config::MAX_WEAPONS];         // 1 - primary fire control, 2 - secondary fire control, 3 - both
    uint8_t WeaponAltControl[config::MAX_WEAPONS];      // 0 - disabled, 1 - keyboard, 2 - mouse, 3 - joystick
    uint8_t WeaponAltControlData[config::MAX_WEAPONS];  // alt control data

    // default systems for default ship
    uint8_t EngineSystem{1};
    uint8_t TargetingSystem{1};
    uint8_t AdvancedProtectionSystem{1};
    uint8_t PowerSystem{1};
    uint8_t TargetingMechanicSystem{1};

    int Money{200};
    int Experience{0};

    uint8_t PrimaryWeaponFireMode{2};       // 1 - shoot a volley, 2 - shoot a burst
    uint8_t SecondaryWeaponFireMode{2};     // 1 - shoot a volley, 2 - shoot a burst

    int OpenLevelNum{0};    // allowed missions
    int LastMission{0};     // last chosen mission

    int ByMissionExperience[config::MAX_MISSIONS];  // experience for each mission
    int MissionReplayCount[config::MAX_MISSIONS];   // how many times mission was replayed

    sPilotProfile();
};

struct sGameConfig {
    unsigned int MenuLanguage{0}; // en
    unsigned int VoiceLanguage{0}; // en
    int FontNumber{0}; // first font from the list

    int MusicVolume{8};     // in-game music volume [0, 10]
    int SoundVolume{10};    // in-game sfx volume [0, 10]
    int VoiceVolume{10};    // in-game voice volume [0, 10]

    int DisplayIndex{0};
    int Width{1280};        // view size width
    int Height{768};        // view size height
    bool Fullscreen{false};  // fullscreen mode (if false - windowed mode)

    // FIXME should be removed, vw_GetInternalResolution() should be used instead in code
    //       also, all directly usage of 1228/1024/768 should be removed as well
    float InternalWidth{1228.0f};   // internal resolution's width
    float InternalHeight{768.0f};   // internal resolution's height

    int VSync{0};
    int Brightness{5};
    int MSAA{0}; // MS anti aliasing
    int CSAA{0}; // CS anti aliasing

    int VisualEffectsQuality{2};    // VisualEffectsQuality is inverted (0 - all effects, 2 - minimum effects)
    int AnisotropyLevel{1};         // textures anisotropic filtering level
    bool UseGLSL120{false};         // 120 (OpenGL 2.1)
    int ShadowMap{0};               // gfx (shadow map)
    int MaxPointLights{3};          // lights point max quantity

    // keyboard
    int KeyBoardLeft{0};
    int KeyBoardRight{0};
    int KeyBoardUp{0};
    int KeyBoardDown{0};
    int KeyBoardPrimary{0};
    int KeyBoardSecondary{0};
    // mouse
    int MousePrimary{0};
    int MouseSecondary{0};
    bool MouseControl{true};
    int ControlSensivity{5};
    // joystick
    int JoystickPrimary{0};
    int JoystickSecondary{1};
    int JoystickNum{0};
    int JoystickDeadZone{2};

    float GameSpeed{1.5f};
    bool ShowFPS{false};
    eWeaponPanelView WeaponPanelView{eWeaponPanelView::full};

    sPilotProfile Profile[config::MAX_PROFILES];

    int LastProfile{-1}; // last used pilot profile

    // tips and hints status
    bool NeedShowHint[config::MAX_HINTS];

    int MenuScript{0}; // last used menu background script number

    sGameConfig();
};


// Get configuration for read only.
const sGameConfig &GameConfig();
// Get configuration for read and write.
sGameConfig &ChangeGameConfig();
// Configure virtual internal resolution by view size.
void ConfigVirtualInternalResolution();
// Load configuration file.
bool LoadXMLConfigFile(bool NeedResetConfig);
// Save configuration file.
void SaveXMLConfigFile();

// Game's difficulty in %, calculated by profile settings (result is cached).
// For more speed, we don't check ProfileNumber for [0, config::MAX_PROFILES) range.
enum class eDifficultyAction {
    Get,
    Update,
    UpdateAll
};
int ProfileDifficulty(int ProfileNumber, eDifficultyAction Action = eDifficultyAction::Get);

} // astromenace namespace
} // viewizard namespace

#endif // CONFIG_CONFIG_H
