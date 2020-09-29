#include "ability.h"
#include "align.h"
#include "battle.h"
#include "bitmap.h"
#include "color.h"
#include "const.h"
#include "data_table.h"
#include "debug.h"
#include "draw.h"
#include "editor.h"
#include "effect.h"
#include "freetype.h"
#include "freetype_wrapper.h"
#include "game.h"
#include "global.h"
#include "image.h"
#include "imgui.h"
#include "input.h"
#include "lang.h"
#include "log.h"
#include "macros.h"
#include "math.h"
#include "memory.h"
#include "meta.h"
#include "meta_print(manual).h"
#include "meta_text_parsing.h"
#include "opengl.h"
#include "oscillating_timer.h"
#include "passive_skill_tree.h"
#include "platform.h"
#include "random.h"
#include "sprite.h"
#include "string.h"
#include "target_class.h"
#include "text_parsing.h"
#include "text_render.h"
#include "timer.h"
#include "traitset.h"
#include "types.h"
#include "unit.h"
#include "utf32string.h"
#include "util.h"
#include "vec.h"

String MetaString(const AbilityTier *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "AbilityTier {\n");
	AppendCString(&string, "  init: %d (bool)\n", s->init);
	AppendCString(&string, "  required_traits: [invalid metadata] (TraitSet)\n", s->required_traits);
	AppendCString(&string, "  effects: [invalid metadata] (Effect)\n", s->effects);
	AppendCString(&string, "}");

	return string;
}

String MetaString(const Ability *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Ability {\n");
	AppendCString(&string, "  init: %d (bool)\n", s->init);
	AppendCString(&string, "  name: [invalid metadata] (String)\n", s->name);
	AppendCString(&string, "  tiers: [invalid metadata] (AbilityTier)\n", s->tiers);
	AppendCString(&string, "}");

	return string;
}

String MetaString(const Editor *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Editor {\n");
	AppendCString(&string, "  init: %d (bool)\n", s->init);
	AppendCString(&string, "  arena: [invalid metadata] (Arena)\n", s->arena);
	AppendCString(&string, "  mode: [invalid metadata] (EditorMode)\n", s->mode);
	AppendCString(&string, "  search_panel_layout: [invalid metadata] (ListPanelLayout)\n", s->search_panel_layout);
	AppendCString(&string, "  panel_scroll_acc: %f (float)\n", s->panel_scroll_acc);
	AppendCString(&string, "  panel_scroll_vel: %f (float)\n", s->panel_scroll_vel);
	AppendCString(&string, "  panel_scroll_pos: %f (float)\n", s->panel_scroll_pos);
	AppendCString(&string, "  panel_scroll_friction: %f (float)\n", s->panel_scroll_friction);
	AppendCString(&string, "  panel_scroll_velocity_minimum: %f (float)\n", s->panel_scroll_velocity_minimum);
	AppendCString(&string, "  active_index: %d (int)\n", s->active_index);
	AppendCString(&string, "  text_cursor_pos: %d (int)\n", s->text_cursor_pos);
	AppendCString(&string, "  input_elements: [invalid metadata] (InputElement)\n", s->input_elements);
	AppendCString(&string, "  temp_ability: [invalid metadata] (Ability)\n", s->temp_ability);
	AppendCString(&string, "  temp_unit_schematic: [invalid metadata] (UnitSchematic)\n", s->temp_unit_schematic);
	AppendCString(&string, "}");

	return string;
}


String MetaString(const Arena *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Arena {\n");
	AppendCString(&string, "  start: %p (void *)\n", s->start);
	AppendCString(&string, "  end: %p (void *)\n", s->end);
	AppendCString(&string, "  current: %p (void *)\n", s->current);
	AppendCString(&string, "}");

	return string;
}

String MetaString(const String *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "String {\n");
	AppendCString(&string, "  length: %d (int)\n", s->length);
	AppendCString(&string, "  max_length: %d (int)\n", s->max_length);
	AppendCString(&string, "  data: %p (char *)\n", s->data);
	AppendCString(&string, "}");

	return string;
}

String MetaString(const TraitSet *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "TraitSet {\n");
	AppendCString(&string, "  vigor: %d (s32)\n", s->vigor);
	AppendCString(&string, "  focus: %d (s32)\n", s->focus);
	AppendCString(&string, "  armor: %d (s32)\n", s->armor);
	AppendCString(&string, "}");

	return string;
}

String MetaString(const Utf32String *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Utf32String {\n");
	AppendCString(&string, "  length: %d (int)\n", s->length);
	AppendCString(&string, "  max_length: %d (int)\n", s->max_length);
	AppendCString(&string, "  data: %p (u32 *)\n", s->data);
	AppendCString(&string, "}");

	return string;
}
