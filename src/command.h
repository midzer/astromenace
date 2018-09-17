/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
	Copyright (c) 2006-2018 Mikhail Kurinnoi, Viewizard


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


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef COMMAND_H
#define COMMAND_H

#include "core/base.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// commands, that should be proceeded after rendering cycle
enum class eCommand {
	DO_NOTHING,
	SWITCH_TO_MAIN_MENU,
	SWITCH_TO_TOP_SCORES,
	SWITCH_TO_INTERFACE,
	SWITCH_TO_OPTIONS,
	SWITCH_TO_CONFCONTROL,
	SWITCH_TO_OPTIONS_ADVANCED,
	SWITCH_TO_INFORMATION,
	SWITCH_TO_CREDITS,
	SWITCH_TO_PROFILE,
	SWITCH_TO_DIFFICULTY,
	SWITCH_TO_MISSION,
	SWITCH_TO_WORKSHOP,
	SWITCH_FROM_MENU_TO_GAME, // also used for mission restart
	SWITCH_FROM_GAME_TO_MISSION_MENU,
	SWITCH_FROM_GAME_TO_MAIN_MENU,
	SWITCH_FROM_GAME_TO_CREDITS
};

class cCommand {
private:
	cCommand() = default;
	~cCommand() = default;

	eCommand Command_{eCommand::DO_NOTHING};

public:
	cCommand(cCommand const&) = delete;
	void operator = (cCommand const&) = delete;

	static cCommand &GetInstance()
	{
		static cCommand Object;
		return Object;
	}

	void Proceed();
	void Set(eCommand Command);
};

} // astromenace namespace
} // viewizard namespace

#endif // COMMAND_H