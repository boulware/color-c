#ifndef GLOBAL_H
#define GLOBAL_H

#include "text_render.h"
#include "imgui.h"
#include "camera.h"

struct Unit;
#include "passive_skill_tree.h"

namespace g
{
	Font default_font;

	Table<Ability> ability_table;
	Table<Breed> breed_table;
	Table<Unit> unit_table;
    Table<Unit> temp_unit_table;
	// Table passive_skill_table;
	// Table passive_node_table;

	PassiveSkillTree passive_tree;

	bool error_flash_increasing = true;
	float error_flash_counter = 0.f;

    Camera ui_camera = {
        .pos = {960.f,540.f},
        .view = {1920.f,1080.f}
    };
};

// Aliases until RemedyBG has namespace support
Table<Ability> *g_ability_table;
Table<Breed> *g_breed_table;
Table<Unit> *g_unit_table;
Table<Unit> *g_temp_unit_table;


#endif