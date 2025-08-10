#include <YYToolkit/YYTK_Shared.hpp>
#include <set>
#include <fstream>
#include <sstream>

#include "Ari.h"
#include "InfoToastsMenu.h"
#include "LoggingUtility.h"
using namespace Aurie;
using namespace YYTK;

const char* const VERSION = "2.0.2";
const char* const MOD_NAME = "Intense Spells";
static const std::string INCREASED_INTENSITY = "Notifications/Mods/IntenseSpells/increaseIntensity";
static const std::string DECREASED_INTENSITY = "Notifications/Mods/IntenseSpells/decreaseIntensity";

static int lastSelectedItem = -1;
static int lastPage = 1;
static bool isFirstPageSelect = true;
static int rain_days = 0;
static int intensity = 0;
static std::vector<RValue> collectedStructs;

RValue& UpdateClock(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const auto original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, "ClockUpdate"));
	// This part is a modified version of the popup done by ArchieUwU in Time of Mistria
	if (GetAsyncKeyState(VK_ADD) & 1)
	{

		InfoToastsMenu::InfoToastsMenu::CreateNotification(INCREASED_INTENSITY);

		intensity += 1;

	}
	else if (GetAsyncKeyState(VK_SUBTRACT) & 1)
	{
		if (intensity <= 0)
			return original(
				Self,
				Other,
				Result,
				ArgumentCount,
				Arguments
			);

		InfoToastsMenu::InfoToastsMenu::CreateNotification(DECREASED_INTENSITY);

		intensity -= 1;
	}
	return original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

}

// Callback hooked into "gml_Script_start_new_weather_event@WeatherManager@Weather", early return if s_weather is -1.
RValue& StartWeatherEvent(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const auto original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, "StartWeatherEvent"));

	if (rain_days == 0)
		return original(
			Self,
			Other,
			Result,
			ArgumentCount,
			Arguments
		);

	RValue weather = 1;
	Arguments[0] = &weather;
	rain_days -= 1;

	return original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	// The debugging I used for this project
	// g_ModuleInterface->Print(CM_LIGHTGREEN, "gml_Script_start_new_weather_event@WeatherManager@Weather: %i", ArgumentCount);
	// g_ModuleInterface->Print(CM_LIGHTGREEN, "gml_Script_start_new_weather_event@WeatherManager@Weather: %i", Arguments[0]->ToInt64());

}


RValue& WeatherTomorrow(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const auto original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, "WeatherTomorrow"));



	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (rain_days == 0)
		return original(
			Self,
			Other,
			Result,
			ArgumentCount,
			Arguments
		);

	Result = 2;

	//g_ModuleInterface->Print(CM_LIGHTGREEN, "gml_Script_weather_tomorrow: %i", ArgumentCount);
	//g_ModuleInterface->Print(CM_LIGHTGREEN, "gml_Script_weather_tomorrow: %i", Result.ToInt64());

	return Result;
}


RValue& CastGrowthSpell(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN double Range
)
{
	CRoom* current_room = nullptr;
	if (!AurieSuccess(g_ModuleInterface->GetCurrentRoomData(current_room)))
	{
		g_ModuleInterface->Print(CM_RED, "Couldn't get the current room using YYTK!");
	}

	double xPos = Other->ToRValue().ToRefMap()["_xx"]->ToDouble() / 8;
	double yPos = Other->ToRValue().ToRefMap()["_yy"]->ToDouble() / 8;

	for (
		CInstance* inst = current_room->GetMembers().m_ActiveInstances.m_First;
		inst != nullptr;
		inst = inst->GetMembers().m_Flink
		)
	{
		auto map = inst->ToRValue().ToRefMap();
		if (!map.contains("node"))
			continue;

		RValue* nodeValue = map["node"];
		if (!nodeValue || nodeValue->GetKindName() != "struct")
			continue;

		auto nodeRefMap = nodeValue->ToRefMap();
		if (!nodeRefMap.contains("prototype"))
			continue;

		RValue* protoVal = nodeRefMap["prototype"];
		if (!protoVal || protoVal->GetKindName() != "struct")
			continue;

		auto protoMap = protoVal->ToRefMap();
		if (!protoMap.contains("is_plant") || !protoMap["is_plant"]->ToBoolean())
			continue;

		if (!nodeRefMap.contains("stage"))
			continue;

		// Position & distance calculation
		double structX = nodeRefMap["top_left_x"]->ToDouble() + 1;
		double structY = nodeRefMap["top_left_y"]->ToDouble() + 1;
		double dx = round(abs(structX - xPos) + 0.5);
		double dy = round(abs(structY - yPos) + 0.5);
		g_ModuleInterface->Print(CM_LIGHTGREEN, "Range: %f %f %f", Range, dx, dy);
		if (dx > Range || dy > Range)
			continue;

		// Get the node from the instance of the crop
		auto innerMap = inst->ToRValue().ToRefMap();
		if (!innerMap.contains("node"))
			continue;

		RValue* nodeVal = innerMap["node"];
		if (!nodeVal || nodeVal->GetKindName() != "struct")
			continue;

		// Prepare script call
		RValue boolValue = false;
		RValue result;
		RValue* args[2] = { nodeVal, &boolValue };

		// Get the script using the YYTK Interface
		CScript* gml_script_level_up_crop = nullptr;
		Aurie::AurieStatus st = g_ModuleInterface->GetNamedRoutinePointer(
			"gml_Script_level_up_crop",
			reinterpret_cast<PVOID*>(&gml_script_level_up_crop)
		);
		// Did we get the script successfully?
		if (!AurieSuccess(st))
			continue;

		// Call gml_Script_level_up_crop
		gml_script_level_up_crop->m_Functions->m_ScriptFunction(
			Self,
			Other,
			Result,
			2,
			args
		);
	}
	return Result;
}

RValue& CastIntenseGrowthSpell(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result
)
{
	RValue current_mana = Ari::Ari::GetCurrentMana();
	if (current_mana.ToInt64() < (2 + intensity))
	{
		InfoToastsMenu::InfoToastsMenu::CreateNotification(DECREASED_INTENSITY);
		intensity = (current_mana.ToInt64() - 2);
		//g_ModuleInterface->Print(CM_LIGHTGREEN, "Set intensity to: %i", intensity);
	}

	Ari::Ari::ModifyMana(static_cast<int>(-1 * intensity));
	CastGrowthSpell(Self, Other, Result, intensity * 2 + 1.0);
	return Result;
}

RValue& CastSpellCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const auto original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, "CastSpellCallback"));

	RValue current_mana = Ari::Ari::GetCurrentMana();


	if (Arguments[0]->ToInt64() == 2)
		return CastIntenseGrowthSpell(Self, Other, Result);

	if (Arguments[0]->ToInt64() == 3)
	{
		original(
			Self,
			Other,
			Result,
			ArgumentCount,
			Arguments
		);
		if (current_mana.ToInt64() < (4 + intensity))
		{
			InfoToastsMenu::InfoToastsMenu::CreateNotification(DECREASED_INTENSITY);
			intensity = current_mana.ToInt64() - 4;
			g_ModuleInterface->Print(CM_LIGHTGREEN, "Set intensity to: %i", intensity);
		}
		Ari::Ari::ModifyMana(static_cast<int>(-1 * intensity));
		rain_days = (intensity + 1);
		g_ModuleInterface->Print(CM_LIGHTGREEN, "It will rain for %i days. %i days come from the intensity of the casted spell.", rain_days, intensity);
	}
	



	return Result;
}

RValue& GrowCropCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const auto original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, "GrowCropCallback"));


	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);



	return Result;
}






EXPORTED AurieStatus ModuleInitialize(
	IN AurieModule* Module,
	IN const fs::path& ModulePath
)
{
	UNREFERENCED_PARAMETER(ModulePath);

	AurieStatus last_status = AURIE_SUCCESS;

	last_status = ObGetInterface(
		"YYTK_Main",
		(AurieInterfaceBase*&)(g_ModuleInterface)
	);

	if (!AurieSuccess(last_status))
	{
		g_ModuleInterface->PrintError(
			__FILE__,
			__LINE__,
			"[Intense Spells v%s] Failed to get YYTK_Main interface! Reason: %s. Is YYToolkit installed?",
			VERSION,
			AurieStatusToString(last_status)
		);

		return last_status;
	}

	last_status = g_ModuleInterface->GetGlobalInstance(&global_instance);

	if (!AurieSuccess(last_status))
	{
		g_ModuleInterface->PrintError(
			__FILE__,
			__LINE__,
			"[Intense Spells v%s] Failed to get the global instance! Reason: %s.",
			VERSION,
			AurieStatusToString(last_status)
		);
	}

	RegisterHook("gml_Script_start_new_weather_event@WeatherManager@Weather", "StartWeatherEvent", StartWeatherEvent);
	//RegisterHook("gml_Script_node_index_for_cell@Grid@Grid", "SeletCellCallback", SeletCellCallback);
	//RegisterHook("gml_Script_pick_node", "FindNodeCallback", FindNodeCallback);
	//RegisterHook("gml_Script_pick_axe@anon@83430@AriFsm@AriFsm", "PickaxeCallback", PickaxeCallback);
	RegisterHook("gml_Script_weather_tomorrow", "WeatherTomorrow", WeatherTomorrow);
	RegisterHook("gml_Script_cast_spell", "CastSpellCallback", CastSpellCallback);
	RegisterHook("gml_Script_level_up_crop", "GrowCropCallback", GrowCropCallback);
	//RegisterHook("gml_Script_set_to_item@CraftingMenu@CraftingMenu", "CraftingTableSetToItemCallback", CraftingTableSetToItemCallback);
	//RegisterHook("gml_Script_select_category@CraftingMenu@CraftingMenu", "SelectCategory", SelectCategory);
	//RegisterHook("gml_Script_on_close@CraftingMenu@CraftingMenu", "CloseCraftingMenuCallback", CloseCraftingMenuCallback);

	//gml_Script_anon@4732@cast_spell@Spells => SoundEffect (String argument)
	//gml_Script_close_and_cast@SpellcastingMenu@SpellcastingMenu => Didn't manage to trigger it
	//gml_Script_anon@170@close_and_cast@SpellcastingMenu@SpellcastingMenu => Didn't manage to trigger it
	//gml_Script_select_spell@SpellcastingMenu@SpellcastingMenu => 1 Argument, number = slot
	// gml_Script_load_spells => 0 Arguments, Result is an array of structs representing the spells
	//
	//RegisterHook("gml_Script_write_crop_to_location", "Exchanger", Exchanger);

	// Taken from Time of Mistria
	RegisterHook("gml_Script_update@Clock@Clock", "ClockUpdate", UpdateClock);

	

	return AURIE_SUCCESS;
}
