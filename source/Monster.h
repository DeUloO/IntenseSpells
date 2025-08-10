#pragma once

#ifndef MONSTER_H_
#define MONSTER_H_
using namespace Aurie;
using namespace YYTK;
#include "Module.h"

namespace Monster {
	void SpawnMonster(CInstance* Self, CInstance* Other, int room_x, int room_y, int monster_id)
	{
		CScript* gml_script_spawn_monster = nullptr;
		g_ModuleInterface->GetNamedRoutinePointer(
			"gml_Script_spawn_monster",
			(PVOID*)&gml_script_spawn_monster
		);

		RValue x = room_x;
		RValue y = room_y;
		RValue monster = monster_id;

		RValue result;
		RValue* x_ptr = &x;
		RValue* y_ptr = &y;
		RValue* monster_ptr = &monster;
		RValue* arguments[3] = { x_ptr, y_ptr, monster_ptr };

		gml_script_spawn_monster->m_Functions->m_ScriptFunction(
			Self,
			Other,
			result,
			3,
			arguments
		);
	}
}


#endif