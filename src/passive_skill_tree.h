#ifndef PASSIVE_SKILL_TREE_H
#define PASSIVE_SKILL_TREE_H

#include "global.h"

struct PassiveSkill
{
	char name[c::max_passive_skill_name_length+1];
};

struct PassiveNode
{
	PassiveSkill *passive_skill;
	PassiveNode *children[c::max_tree_node_children];
};

struct PassiveSkillTree
{
	PassiveNode *root;
};

void DrawPassiveSkillTree(Vec2f pos, PassiveSkillTree tree);

namespace passive
{
	const float child_vert_advance = 100.f;
	const float child_hori_offset = 100.f;
};

void DrawPassiveNodeChildren(Vec2f origin, PassiveNode *node);
void DrawPassiveSkillTree(Vec2f origin, PassiveSkillTree tree);

#endif