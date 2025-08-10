#pragma once
#include <YYToolkit/YYTK_Shared.hpp>
#include <fstream>
#include <unordered_set>
#include <string_view>

using namespace Aurie;
using namespace YYTK;

static std::unordered_set<void*> pointerSet;
// These can be left in but severly bloat up any dump! Expect sizes of 100-200MB if you allow all keys!
static const std::unordered_set<std::string_view> disallowedKeys = {
	"node_terrain_tile",
	"node_terrain_kind",
	"node_is_room_editor_collision",
	"node_object_id",
	"node_collideable",
	"node_terrain_is_watered",
	"node_rug_parent",
	"node_pathfinding_cost",
	"node_flags",
	"node_force",
	"node_terrain_variant",
	"node_terrain_is_watered",
	"node_rug_id",
	"node_is_room_editor_collision",
	"node_top_left_y",
	"node_top_left_x",
	"node_terrain_ground_kind",
	"node_pathfinding_sitting",
	"node_parent",
	"maximum_item_counts",
	"node_footstep_kind",
	"node_can_jump_over"
};
static std::string dump_name = "dump.txt";
static std::ofstream dumpFile;
bool IsDisallowedKey(const std::string& key) {
	return disallowedKeys.find(key) != disallowedKeys.end();
}

void DumpInstanceRecursive(
	IN RValue& Instance,
	IN int32_t Depth
);

void DumpArrayRecursive(
	IN const char* Name,
	IN const RValue& Value,
	IN int32_t Depth
);

static void LogToFile(const std::string& message)
{
	if (!dumpFile.is_open())
		dumpFile.open(dump_name, std::ios::app);
	if (dumpFile.is_open())
		dumpFile << message << std::endl;
}

static void ResetDumpFile()
{
	if (dumpFile.is_open())
		dumpFile.close();

}


CScript* TryLookupScriptByFunctionPointer(
	IN PFUNC_YYGMLScript ScriptFunction
)
{
	AurieStatus last_status = AURIE_SUCCESS;

	int script_index = 0;
	while (AurieSuccess(last_status))
	{
		CScript* script = nullptr;

		last_status = g_ModuleInterface->GetScriptData(
			script_index,
			script
		);

		if (!AurieSuccess(last_status))
			break;

		if (script->m_Functions)
		{
			if (script->m_Functions->m_ScriptFunction == ScriptFunction)
				return script;
		}

		script_index++;
	}
}

void DumpRValue(
	IN int32_t Depth,
	IN const char* Name,
	IN RValue& Value
)
{
	std::string prefix = std::string(Depth * 2, ' ');
	char buffer[1024];

	switch (Value.m_Kind)
	{
	case VALUE_REAL:
	case VALUE_INT64:
	case VALUE_INT32:
	case VALUE_BOOL:
		snprintf(buffer, sizeof(buffer), "%s - [%s] %s = %.2f", prefix.c_str(), Value.GetKindName().c_str(), Name, Value.ToDouble());
		LogToFile(buffer);
		break;
	case VALUE_OBJECT:
		snprintf(buffer, sizeof(buffer), "%s - [%s] %s = %p", prefix.c_str(), Value.GetKindName().c_str(), Name, Value.ToPointer());
		LogToFile(buffer);

		// Avoid loops
		if (pointerSet.contains(Value.ToPointer()))
			break;
		pointerSet.insert(Value.ToPointer());

		DumpInstanceRecursive(Value, Depth + 1);
		break;
	case VALUE_STRING:
		snprintf(buffer, sizeof(buffer), "%s - [%s] %s = '%s'", prefix.c_str(), Value.GetKindName().c_str(), Name, Value.ToCString());
		LogToFile(buffer);
		break;
	case VALUE_ARRAY:
		if (IsDisallowedKey(Name))
			break;
		snprintf(buffer, sizeof(buffer), "%s - [%s] %s", prefix.c_str(), Value.GetKindName().c_str(), Name);
		LogToFile(buffer);
		DumpArrayRecursive(Name, Value, Depth + 1);
		break;
	default:
		snprintf(buffer, sizeof(buffer), "%s - [%s] %s", prefix.c_str(), Value.GetKindName().c_str(), Name);
		LogToFile(buffer);
		break;
	}
}

void DumpArrayRecursive(
	IN const char* Name,
	IN const RValue& Value,
	IN int32_t Depth
)
{
	auto vector = Value.ToVector();

	for (int i = 0; i < vector.size(); i++)
	{
		std::string name = Name;
		name.append("[");
		name.append(std::to_string(i));
		name.append("]");

		DumpRValue(Depth, name.c_str(), vector[i]);
	}
}

void DumpInstanceRecursive(
	IN RValue& Instance,
	IN int32_t Depth
)
{
	auto key_value_map = Instance.ToRefMap();

	for (auto& [key, value] : key_value_map)
	{
		DumpRValue(Depth, key.c_str(), *value);
	}
}

// This dumps the passed Instance into the same folder as the FieldsOfMistria.exe with the passed name.
void DumpInstance(
	IN RValue& Instance,
	IN int32_t Depth,
	IN std::string dump_file_name
)
{
	dump_name = dump_file_name;
	auto key_value_map = Instance.ToRefMap();

	for (auto& [key, value] : key_value_map)
	{
		DumpRValue(Depth, key.c_str(), *value);
	}
	LogToFile("--------------------------------");
	dump_name = "dump.txt";
	ResetDumpFile();
}
