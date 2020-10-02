#include "passive_skill_tree.h"

void
DrawPassiveNodeChildren(Vec2f origin, PassiveNode *node)
{
	ImguiContainer container = {};
	container.pos = origin;
	container.button_layout = c::def_button_layout;
	container.button_layout.align = c::align_center;
	SetActiveContainer(&container);

	int child_count = 0;
	for(PassiveNode *child : node->children)
	{
		if(child != nullptr) ++child_count;
	}

	if(child_count == 0)
	{
		return;
	}
	else if(child_count == 1)
	{
		SetActiveContainer(&container);
		PassiveNode *child = node->children[0];
		container.pos.y += passive::child_vert_advance;
		Button(child->passive_skill->name);

		DrawPassiveNodeChildren(container.pos, child);
	}
	else if(child_count == 2)
	{
		// Left child
		SetActiveContainer(&container);
		PassiveNode *left = node->children[0];
		container.pos += {-passive::child_hori_offset, passive::child_vert_advance};
		Button(left->passive_skill->name);
		container.pen = {0.f,0.f};

		DrawPassiveNodeChildren(container.pos, left);

		// Right child
		SetActiveContainer(&container);
		container.pos = origin;
		PassiveNode *right = node->children[1];
		container.pos += {+passive::child_hori_offset, passive::child_vert_advance};
		Button(right->passive_skill->name);
		container.pen = {0.f,0.f};

		DrawPassiveNodeChildren(container.pos, right);
	}
}

void
DrawPassiveSkillTree(Vec2f origin, PassiveSkillTree tree)
{
	ImguiContainer container = {};
	container.pos = origin;
	container.button_layout = c::def_button_layout;
	container.button_layout.align = c::align_center;
	SetActiveContainer(&container);
	Button(tree.root->passive_skill->name);

	// TextLayout text_layout = c::def_text_layout;
	// text_layout.align = c::align_center;
	// Vec2f text_size = DrawText(text_layout, origin, tree.root->passive_skill->name);
	// DrawUnfilledRect({origin-0.5f*text_size, text_size}, c::white);

	DrawPassiveNodeChildren(origin, tree.root);
}

		//			E
		//         / \
		//        P   C
		//       / \
		//      B   S
		//

	// // Passive skill tree node table
	// g::passive_node_table = AllocTable(sizeof(PassiveNode), c::passive_node_table_partition_size);
	// {
	// 	//			E
	// 	//         / \
	// 	//        P   C
	// 	//       / \
	// 	//      B   S
	// 	//

	// 	PassiveNode *e_node = (PassiveNode*)CreateEntry(&g::passive_node_table);
	// 	e_node->passive_skill = GetEntryByName<PassiveSkill>(g::passive_skill_table, "Equilibrium");
	// 	PassiveNode *p_node = (PassiveNode*)CreateEntry(&g::passive_node_table);
	// 	p_node->passive_skill = GetEntryByName<PassiveSkill>(g::passive_skill_table, "Potency");
	// 	PassiveNode *c_node = (PassiveNode*)CreateEntry(&g::passive_node_table);
	// 	c_node->passive_skill = GetEntryByName<PassiveSkill>(g::passive_skill_table, "Constitution");
	// 	PassiveNode *b_node = (PassiveNode*)CreateEntry(&g::passive_node_table);
	// 	b_node->passive_skill = GetEntryByName<PassiveSkill>(g::passive_skill_table, "Bravery");
	// 	PassiveNode *s_node = (PassiveNode*)CreateEntry(&g::passive_node_table);
	// 	s_node->passive_skill = GetEntryByName<PassiveSkill>(g::passive_skill_table, "Stoicism");

	// 	e_node->children[0] = p_node;
	// 	e_node->children[1] = c_node;
	// 	p_node->children[0] = b_node;
	// 	p_node->children[1] = s_node;

	// 	g::passive_tree.root = e_node;
	// }