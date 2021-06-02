#ifndef TASK_H
#define TASK_H

#include <unordered_map>

enum class TaskType
{
	DirectBlend,
	OpaqueBlend,
	Tiling
};

enum class RegionType
{
	Rect, 
	File
};

extern std::unordered_map<std::string, TaskType> strToTaskType;
extern std::unordered_map<std::string, RegionType> strToRegionType;

#endif