#ifndef CONST_H
#define CONST_H

#include "color.h"

namespace c
{
	const int window_width = 800;
	const int window_height = 600;

	const int ability_table_partition_size = 20; // partition size in number of entities (not bytes)
	const int unit_table_partition_size = 20;

	const char whitespace[] = " \t\n\v\f\r";
	const int max_s32_digits = 11; // including '-'
	const u32 medium_text = 24;

	// Colors
	const Color red 	= {1.f,0.f,0.f};
	const Color green 	= {0.f,1.f,0.f};
	const Color blue 	= {0.f,0.f,1.f};
	const Color black 	= {0.f,0.f,0.f};
	const Color white 	= {1.f,1.f,1.f};
	const Color grey 	= {0.5f,0.5f,0.5f};
	const Color lt_grey = {0.7f,0.7f,0.7f};
	const Color dk_grey = {0.2f,0.2f,0.2f};
	const Color yellow	= {1.f,1.f,0.f};
	const Color orange	= {1.f,0.4f,0.f};

	// Gameplay
	const int moveset_max_size = 4;
	const int max_party_size = 4;
	const int max_target_count = 2*max_party_size;
};

#endif