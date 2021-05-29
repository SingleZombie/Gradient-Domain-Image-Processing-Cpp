#include "Task.h"

std::unordered_map<std::string, TaskType> strToTaskType =
{
	{"DirectBlend", TaskType::DirectBlend},
{"OpaqueBlend", TaskType::OpaqueBlend},
{"Tiling", TaskType::Tiling}
};

std::unordered_map<std::string, RegionType> strToRegionType =
{
	{"Rect", RegionType::Rect},
{"File", RegionType::File}
};