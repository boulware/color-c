#ifndef GLOBAL_H
#define GLOBAL_H

#include "text_render.h"
#include "imgui.h"
#include "data_table.h"
#include "unit.h"
#include "passive_skill_tree.h"

namespace g
{
	Font default_font;

	DataTable ability_table;
	DataTable unit_schematic_table;
	DataTable unit_table;
	DataTable passive_skill_table;
	DataTable passive_node_table;

	PassiveSkillTree passive_tree;

	bool error_flash_increasing = true;
	float error_flash_counter = 0.f;
};

#endif