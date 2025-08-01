#include <YYToolkit/YYTK_Shared.hpp>
#include <set>
using namespace Aurie;
using namespace YYTK;
static YYTKInterface* g_ModuleInterface = nullptr;

static const char* const VERSION = "1.2.0";
static const char* const GML_SCRIPT_CREATE_NOTIFICATION = "gml_Script_create_notification";
static const std::string INCREASED_INTENSITY = "Notifications/Mods/IntenseSpells/increaseIntensity";
static const std::string DECREASED_INTENSITY = "Notifications/Mods/IntenseSpells/decreaseIntensity";


static int rain_days = 0;
static int intensity = 0;

// Taken from AnnaNomolys' StatueOfBoons mod
void CreateNotification(std::string notification_localization_str, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_create_notification = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CREATE_NOTIFICATION,
		(PVOID*)&gml_script_create_notification
	);

	RValue result;
	RValue notification = RValue(notification_localization_str);
	RValue* notification_ptr = &notification;
	gml_script_create_notification->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &notification_ptr }
	);
}


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

		CreateNotification(INCREASED_INTENSITY, Self, Other);

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

		CreateNotification(DECREASED_INTENSITY, Self, Other);

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

	CInstance* global_instance = nullptr;
	if (!AurieSuccess(g_ModuleInterface->GetGlobalInstance(&global_instance)))
	{
		return original(
			Self,
			Other,
			Result,
			ArgumentCount,
			Arguments
		);
	}

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
	g_ModuleInterface->Print(CM_LIGHTGREEN, "Rainy days left: %i", rain_days);

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

	CInstance* global_instance = nullptr;
	if (!AurieSuccess(g_ModuleInterface->GetGlobalInstance(&global_instance)))
	{
		return original(
			Self,
			Other,
			Result,
			ArgumentCount,
			Arguments
		);
	}


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

void ModifyMana(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_modify_mana = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		"gml_Script_modify_mana@Ari@Ari",
		(PVOID*)&gml_script_modify_mana
	);

	RValue result;
	RValue mana_modifier = value;
	RValue* mana_modifier_ptr = &mana_modifier;

	gml_script_modify_mana->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &mana_modifier_ptr }
	);
}

RValue GetCurrentMana(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_mana = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		"gml_Script_get_mana@Ari@Ari",
		(PVOID*)&gml_script_get_mana
	);

	RValue current_mana;
	gml_script_get_mana->m_Functions->m_ScriptFunction(
		Self,
		Other,
		current_mana,
		0,
		nullptr
	);

	return current_mana;
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

	CInstance* global_instance = nullptr;
	if (!AurieSuccess(g_ModuleInterface->GetGlobalInstance(&global_instance)))
	{
		return original(
			Self,
			Other,
			Result,
			ArgumentCount,
			Arguments
		);
	}

	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	RValue current_mana = GetCurrentMana((global_instance->GetRefMember("__ari"))->ToInstance(), Self);
	
	
	if (Arguments[0]->ToInt64() == 3)
	{
		if (current_mana.ToInt64() < (4 + intensity))
		{
			CreateNotification(DECREASED_INTENSITY, Self, Other);
			intensity = current_mana.ToInt64() - 4;
			g_ModuleInterface->Print(CM_LIGHTGREEN, "Set intensity to: %i", intensity);
		}
		ModifyMana((global_instance->GetRefMember("__ari"))->ToInstance(), Self, static_cast<int>(-1 * intensity));
		rain_days = (intensity + 1);
		g_ModuleInterface->Print(CM_LIGHTGREEN, "It will rain for %i days. %i days come from the intensity of the casted spell.", rain_days, intensity);
	}
	//g_ModuleInterface->Print(CM_LIGHTGREEN, "castSpell: %i", ArgumentCount);
	//g_ModuleInterface->Print(CM_LIGHTGREEN, "castSpell: %i", Arguments[0]->ToInt64());
	



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
			"[Forecast v%s] Failed to get YYTK_Main interface! Reason: %s. Is YYToolkit installed?",
			VERSION,
			AurieStatusToString(last_status)
		);

		return last_status;
	}


	CScript* start_weather_event = nullptr;
	last_status = g_ModuleInterface->GetNamedRoutinePointer(
		"gml_Script_start_new_weather_event@WeatherManager@Weather",
		reinterpret_cast<PVOID*>(&start_weather_event)
	);

	if (!AurieSuccess(last_status))
	{
		g_ModuleInterface->PrintError(
			__FILE__,
			__LINE__,
			"[Forecast v%s] Failed to find gml_Script_start_new_weather_event@WeatherManager@Weather! Reason: %s",
			VERSION,
			AurieStatusToString(last_status)
		);

		return last_status;
	}

	last_status = MmCreateHook(
		g_ArSelfModule,
		"StartWeatherEvent",
		start_weather_event->m_Functions->m_ScriptFunction,
		StartWeatherEvent,
		nullptr
	);

	if (!AurieSuccess(last_status))
	{
		g_ModuleInterface->PrintError(
			__FILE__,
			__LINE__,
			"[Forecast v%s] Failed to hook gml_Script_start_new_weather_event@WeatherManager@Weather! Reason: %s",
			VERSION,
			AurieStatusToString(last_status)
		);

		return last_status;
	}

	CScript* weather_tomorrow = nullptr;
	last_status = g_ModuleInterface->GetNamedRoutinePointer(
		"gml_Script_weather_tomorrow",
		reinterpret_cast<PVOID*>(&weather_tomorrow)
	);

	if (!AurieSuccess(last_status))
	{
		g_ModuleInterface->PrintError(
			__FILE__,
			__LINE__,
			"[Forecast v%s] Failed to find gml_Script_weather_tomorrow! Reason: %s",
			VERSION,
			AurieStatusToString(last_status)
		);

		return last_status;
	}

	last_status = MmCreateHook(
		g_ArSelfModule,
		"WeatherTomorrow",
		weather_tomorrow->m_Functions->m_ScriptFunction,
		WeatherTomorrow,
		nullptr
	);

	if (!AurieSuccess(last_status))
	{
		g_ModuleInterface->PrintError(
			__FILE__,
			__LINE__,
			"[Forecast v%s] Failed to hook gml_Script_weather_tomorrow! Reason: %s",
			VERSION,
			AurieStatusToString(last_status)
		);

		return last_status;
	}

	CScript* cast_spell = nullptr;
	last_status = g_ModuleInterface->GetNamedRoutinePointer(
		"gml_Script_cast_spell",
		reinterpret_cast<PVOID*>(&cast_spell)
	);

	if (!AurieSuccess(last_status))
	{
		g_ModuleInterface->PrintError(
			__FILE__,
			__LINE__,
			"[Forecast v%s] Failed to find gml_Script_cast_spell! Reason: %s",
			VERSION,
			AurieStatusToString(last_status)
		);

		return last_status;
	}

	last_status = MmCreateHook(
		g_ArSelfModule,
		"CastSpellCallback",
		cast_spell->m_Functions->m_ScriptFunction,
		CastSpellCallback,
		nullptr
	);

	if (!AurieSuccess(last_status))
	{
		g_ModuleInterface->PrintError(
			__FILE__,
			__LINE__,
			"[Forecast v%s] Failed to hook gml_Script_cast_spell! Reason: %s",
			VERSION,
			AurieStatusToString(last_status)
		);

		return last_status;
	}


	// Taken from Time of Mistria
	CScript* clock_update_script = nullptr;
	last_status = g_ModuleInterface->GetNamedRoutinePointer(
		"gml_Script_update@Clock@Clock",
		reinterpret_cast<PVOID*>(&clock_update_script)
	);

	if (!AurieSuccess(last_status))
	{
		g_ModuleInterface->PrintError(
			__FILE__,
			__LINE__,
			"[Forecast v%s] Failed to find gml_Script_update@Clock@Clock! Reason: %s",
			VERSION,
			AurieStatusToString(last_status)
		);

		return last_status;
	}

	last_status = MmCreateHook(
		g_ArSelfModule,
		"ClockUpdate",
		clock_update_script->m_Functions->m_ScriptFunction,
		UpdateClock,
		nullptr
	);

	if (!AurieSuccess(last_status))
	{
		g_ModuleInterface->PrintError(
			__FILE__,
			__LINE__,
			"[Forecast v%s] Failed to set a hook on gml_Script_update@Clock@Clock! Reason: %s",
			VERSION,
			AurieStatusToString(last_status)
		);

		return last_status;
	}

	return AURIE_SUCCESS;
}
