#pragma once

#ifndef INFO_TOASTS_H_
#define INFO_TOASTS_H_
using namespace Aurie;
using namespace YYTK;
#include "Module.h"

// Documented scripts
static const char* const GML_SCRIPT_CREATE_NOTIFICATION = "gml_Script_create_notification";

// Undocumented scripts


// Subcategory named InfoToastsMenu
namespace InfoToastsMenu {
	namespace InfoToastsMenu {
		// Taken from AnnaNomolys' StatueOfBoons mod
		void CreateNotification(std::string notification_localization_str)
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
				NULL,
				NULL,
				result,
				1,
				{ &notification_ptr }
			);
		}
	}
}

#endif