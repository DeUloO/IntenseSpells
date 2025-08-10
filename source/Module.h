#pragma once

#ifndef MODULE_INIT_H_
#define MODULE_INIT_H_
using namespace Aurie;
using namespace YYTK;
#include <YYToolkit/YYTK_Shared.hpp>

static YYTKInterface* g_ModuleInterface = nullptr;
static CInstance* global_instance = nullptr;
extern const char* const VERSION;
extern const char* const MOD_NAME;

template <typename T>
AurieStatus RegisterHook(const std::string& script_name, const std::string& trampoline_name, T callback)
{
	CScript* target_script = nullptr;

	AurieStatus status = g_ModuleInterface->GetNamedRoutinePointer(
		script_name.c_str(),
		reinterpret_cast<PVOID*>(&target_script)
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->PrintError(
			__FILE__,
			__LINE__,
			"[%s %s] Failed to find %s! Reason: %s",
			VERSION,
			MOD_NAME,
			script_name.c_str(),
			AurieStatusToString(status)
		);
		return status;
	}

	status = MmCreateHook(
		g_ArSelfModule,
		trampoline_name.c_str(),
		target_script->m_Functions->m_ScriptFunction,
		reinterpret_cast<PVOID>(callback),
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->PrintError(
			__FILE__,
			__LINE__,
			"[%s %s] Failed to hook %s! Reason: %s",
			VERSION,
			MOD_NAME,
			script_name.c_str(),
			AurieStatusToString(status)
		);
	}

	return status;
}


PFUNC_YYGMLScript GetHookTrampoline(AurieModule* Module, std::string_view HookIdentifier)
{
	PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(
		MmGetHookTrampoline(Module, HookIdentifier.data())
		);

	return original;
}


#endif
