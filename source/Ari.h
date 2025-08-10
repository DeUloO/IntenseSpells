#pragma once

#ifndef ARI_H_
#define ARI_H_
using namespace Aurie;
using namespace YYTK;
#include "Module.h"

namespace Ari {
	namespace Ari {

		// Modify

		void ModifyHealth(int value)
		{
			CScript* gml_script_modify_health = nullptr;
			g_ModuleInterface->GetNamedRoutinePointer(
				"gml_Script_modify_health@Ari@Ari",
				(PVOID*)&gml_script_modify_health
			);
			g_ModuleInterface->Print(CM_LIGHTGREEN, "TEST");

			RValue result;
			RValue health_modifier = value;
			RValue* health_modifier_ptr = &health_modifier;

			CInstance* ari = global_instance->GetRefMember("__ari")->ToInstance();

			gml_script_modify_health->m_Functions->m_ScriptFunction(
				ari,
				NULL,
				result,
				1,
				{ &health_modifier_ptr }
			);
		}

		void ModifyStamina(int value)
		{
			CScript* gml_script_modify_stamina = nullptr;
			g_ModuleInterface->GetNamedRoutinePointer(
				"gml_Script_modify_health@Ari@Ari",
				(PVOID*)&gml_script_modify_stamina
			);

			RValue result;
			RValue stamina_modifier = value;
			RValue* stamina_modifier_ptr = &stamina_modifier;

			CInstance* ari = global_instance->GetRefMember("__ari")->ToInstance();

			gml_script_modify_stamina->m_Functions->m_ScriptFunction(
				ari,
				NULL,
				result,
				1,
				{ &stamina_modifier_ptr }
			);
		}

		void ModifyMana(int value)
		{
			CScript* gml_script_modify_mana = nullptr;
			g_ModuleInterface->GetNamedRoutinePointer(
				"gml_Script_modify_mana@Ari@Ari",
				(PVOID*)&gml_script_modify_mana
			);

			RValue result;
			RValue mana_modifier = value;
			RValue* mana_modifier_ptr = &mana_modifier;

			CInstance* ari = global_instance->GetRefMember("__ari")->ToInstance();

			gml_script_modify_mana->m_Functions->m_ScriptFunction(
				ari,
				NULL,
				result,
				1,
				{ &mana_modifier_ptr }
			);
		}

		// Getter

		RValue GetMaxHealth()
		{
			CScript* gml_script_get_max_health = nullptr;
			g_ModuleInterface->GetNamedRoutinePointer(
				"gml_Script_get_max_health@Ari@Ari",
				(PVOID*)&gml_script_get_max_health
			);

			CInstance* ari = global_instance->GetRefMember("__ari")->ToInstance();

			RValue max_health;
			gml_script_get_max_health->m_Functions->m_ScriptFunction(
				ari,
				NULL,
				max_health,
				0,
				nullptr
			);

			return max_health;
		}

		RValue GetMaxStamina()
		{
			CScript* gml_Script_get_max_stamina = nullptr;
			g_ModuleInterface->GetNamedRoutinePointer(
				"gml_Script_get_max_stamina@Ari@Ari",
				(PVOID*)&gml_Script_get_max_stamina
			);

			CInstance* ari = global_instance->GetRefMember("__ari")->ToInstance();

			RValue max_health;
			gml_Script_get_max_stamina->m_Functions->m_ScriptFunction(
				ari,
				NULL,
				max_health,
				0,
				nullptr
			);

			return max_health;
		}

		RValue GetMaxMana()
		{
			return RValue(16);
		}


		RValue GetCurrentHealth()
		{
			CScript* gml_script_get_health = nullptr;
			g_ModuleInterface->GetNamedRoutinePointer(
				"gml_Script_get_health@Ari@Ari",
				(PVOID*)&gml_script_get_health
			);

			CInstance* ari = global_instance->GetRefMember("__ari")->ToInstance();

			RValue current_health;
			gml_script_get_health->m_Functions->m_ScriptFunction(
				ari,
				NULL,
				current_health,
				0,
				nullptr
			);

			return current_health;
		}

		RValue GetCurrentStamina()
		{
			CScript* gml_script_get_stamina = nullptr;
			g_ModuleInterface->GetNamedRoutinePointer(
				"gml_Script_get_stamina@Ari@Ari",
				(PVOID*)&gml_script_get_stamina
			);

			CInstance* ari = global_instance->GetRefMember("__ari")->ToInstance();

			RValue current_stamina;
			gml_script_get_stamina->m_Functions->m_ScriptFunction(
				ari,
				NULL,
				current_stamina,
				0,
				nullptr
			);

			return current_stamina;
		}

		RValue GetCurrentMana()
		{
			CScript* gml_script_get_mana = nullptr;
			g_ModuleInterface->GetNamedRoutinePointer(
				"gml_Script_get_mana@Ari@Ari",
				(PVOID*)&gml_script_get_mana
			);

			CInstance* ari = global_instance->GetRefMember("__ari")->ToInstance();

			RValue current_mana;
			gml_script_get_mana->m_Functions->m_ScriptFunction(
				ari,
				NULL,
				current_mana,
				0,
				nullptr
			);

			return current_mana;
		}

		// Setter

		void SetHealth(int value)
		{
			CScript* gml_script_set_health = nullptr;
			g_ModuleInterface->GetNamedRoutinePointer(
				"gml_Script_set_health@Ari@Ari",
				(PVOID*)&gml_script_set_health
			);

			RValue result;
			RValue health_value = value;
			RValue* health_value_ptr = &health_value;

			CInstance* ari = global_instance->GetRefMember("__ari")->ToInstance();

			gml_script_set_health->m_Functions->m_ScriptFunction(
				ari,
				NULL,
				result,
				1,
				{ &health_value_ptr }
			);
		}

		void SetStamina(int value)
		{
			CScript* gml_script_set_stamina = nullptr;
			g_ModuleInterface->GetNamedRoutinePointer(
				"gml_Script_set_stamina@Ari@Ari",
				(PVOID*)&gml_script_set_stamina
			);

			RValue result;
			RValue stamina_value = value;
			RValue* stamina_value_ptr = &stamina_value;

			CInstance* ari = global_instance->GetRefMember("__ari")->ToInstance();

			gml_script_set_stamina->m_Functions->m_ScriptFunction(
				ari,
				NULL,
				result,
				1,
				{ &stamina_value_ptr }
			);
		}


		void SetMana(int value)
		{
			CScript* gml_script_set_mana = nullptr;
			g_ModuleInterface->GetNamedRoutinePointer(
				"gml_Script_set_mana@Ari@Ari",
				(PVOID*)&gml_script_set_mana
			);

			RValue result;
			RValue mana_value = value;
			RValue* mana_value_ptr = &mana_value;

			CInstance* ari = global_instance->GetRefMember("__ari")->ToInstance();

			gml_script_set_mana->m_Functions->m_ScriptFunction(
				ari,
				NULL,
				result,
				1,
				{ &mana_value_ptr }
			);
		}

	}
}


#endif