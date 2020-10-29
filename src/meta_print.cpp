#include "meta_print.h"

#include "ability.h"
#include "ai_explorer.h"
#include "align.h"
#include "array.h"
#include "battle.h"
#include "better_text_parsing.h"
#include "bitmap.h"
#include "buffer.h"
#include "camera.h"
#include "campaign.h"
#include "color.h"
#include "const.h"
#include "debug.h"
#include "debug_overlay.h"
#include "draw.h"
#include "editor.h"
#include "effect.h"
#include "enemy_ai.h"
#include "fixed_array.h"
#include "font_loading.h"
#include "frametimes_draw.h"
#include "freetype.h"
#include "freetype_wrapper.h"
#include "game.h"
#include "game_state.h"
#include "generate_node_graph_params.h"
#include "geometry.h"
#include "global.h"
#include "image.h"
#include "imgui.h"
#include "input.h"
#include "input_vk_constants.h"
#include "intent.h"
#include "keybinds.h"
#include "lang.h"
#include "log.h"
#include "macros.h"
#include "main_menu.h"
#include "map.h"
#include "math.h"
#include "memory.h"
#include "memory_draw.h"
#include "meta.h"
#include "node_graph.h"
#include "opengl.h"
#include "options_menu.h"
#include "oscillating_timer.h"
#include "passive_skill_tree.h"
#include "permutation.h"
#include "platform.h"
#include "pool.h"
#include "random.h"
#include "ring_buffer.h"
#include "room.h"
#include "sprite.h"
#include "string.h"
#include "table.h"
#include "table_draw.h"
#include "target_class.h"
#include "test_mode.h"
#include "text_parsing.h"
#include "text_render.h"
#include "timer.h"
#include "traitset.h"
#include "types.h"
#include "unit.h"
#include "utf32string.h"
#include "util.h"
#include "vec.h"
#include "work_entry.h"

// ---------------FILE START---------------
// ability.h
// ------------------------------------------

String MetaString(const AbilityTier *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "AbilityTier {\n");

	AppendCString(&string, "  required_traits: ");
	AppendString(&string, MetaString(&s->required_traits));
	AppendCString(&string, "(TraitSet)\n");

	AppendCString(&string, "  target_class: ");
	AppendString(&string, MetaString(&s->target_class));
	AppendCString(&string, "(TargetClass)\n");

	AppendCString(&string, "  effects_: ");
	AppendString(&string, MetaString(&s->effects_));
	AppendCString(&string, "(Array<Effect>)\n");

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

	AppendCString(&string, "  name: ");
	AppendString(&string, MetaString(&s->name));
	AppendCString(&string, "(String)\n");

	AppendCString(&string, "  tiers: ");
	AppendString(&string, MetaString(&s->tiers));
	AppendCString(&string, "(Array<AbilityTier>)\n");

	AppendCString(&string, "  tier_potentials: ");
	AppendString(&string, MetaString(&s->tier_potentials));
	AppendCString(&string, "(Array<float>)\n");

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// ai_explorer.h
// ------------------------------------------

String MetaString(const AiExplorer *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "AiExplorer {\n");

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// align.h
// ------------------------------------------

String MetaString(const AlignX *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "AlignX::");
	switch(*s)
	{
		case(AlignX::left): {
			AppendCString(&string, "left");
		} break;
		case(AlignX::center): {
			AppendCString(&string, "center");
		} break;
		case(AlignX::right): {
			AppendCString(&string, "right");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

String MetaString(const AlignY *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "AlignY::");
	switch(*s)
	{
		case(AlignY::top): {
			AppendCString(&string, "top");
		} break;
		case(AlignY::center): {
			AppendCString(&string, "center");
		} break;
		case(AlignY::bottom): {
			AppendCString(&string, "bottom");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

String MetaString(const Align *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Align {\n");

	AppendCString(&string, "  x: ");
	AppendString(&string, MetaString(&s->x));
	AppendCString(&string, "(AlignX)\n");

	AppendCString(&string, "  y: ");
	AppendString(&string, MetaString(&s->y));
	AppendCString(&string, "(AlignY)\n");

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// array.h
// ------------------------------------------


// ---------------FILE START---------------
// battle.h
// ------------------------------------------

String MetaString(const BattleEvent *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "BattleEvent {\n");

	AppendCString(&string, "  caster_id: ");
	AppendString(&string, MetaString(&s->caster_id));
	AppendCString(&string, "(Id<Unit>)\n");

	AppendCString(&string, "  target_id: ");
	AppendString(&string, MetaString(&s->target_id));
	AppendCString(&string, "(Id<Unit>)\n");

	AppendCString(&string, "  trait_changes: ");
	AppendString(&string, MetaString(&s->trait_changes));
	AppendCString(&string, "(TraitSet)\n");

	AppendCString(&string, "}");

	return string;
}

String MetaString(const BattlePhase *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "BattlePhase::");
	switch(*s)
	{
		case(BattlePhase::invalid): {
			AppendCString(&string, "invalid");
		} break;
		case(BattlePhase::start): {
			AppendCString(&string, "start");
		} break;
		case(BattlePhase::player_turn): {
			AppendCString(&string, "player_turn");
		} break;
		case(BattlePhase::end_of_player_turn): {
			AppendCString(&string, "end_of_player_turn");
		} break;
		case(BattlePhase::enemy_turn): {
			AppendCString(&string, "enemy_turn");
		} break;
		case(BattlePhase::end_of_enemy_turn): {
			AppendCString(&string, "end_of_enemy_turn");
		} break;
		case(BattlePhase::end): {
			AppendCString(&string, "end");
		} break;
		case(BattlePhase::COUNT): {
			AppendCString(&string, "COUNT");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

String MetaString(const BattleState *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "BattleState {\n");

	AppendCString(&string, "  finished: %d (bool)\n", s->finished);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const Battle *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Battle {\n");

	AppendCString(&string, "  arena_id: ");
	AppendString(&string, MetaString(&s->arena_id));
	AppendCString(&string, "(PoolId<Arena>)\n");

	AppendCString(&string, "  ai_arena_id: ");
	AppendString(&string, MetaString(&s->ai_arena_id));
	AppendCString(&string, "(PoolId<Arena>)\n");

	AppendCString(&string, "  hud: ");
	AppendString(&string, MetaString(&s->hud));
	AppendCString(&string, "(Rect)\n");

	AppendCString(&string, "  phase: ");
	AppendString(&string, MetaString(&s->phase));
	AppendCString(&string, "(BattlePhase)\n");

	AppendCString(&string, "  selected_unit_id: ");
	AppendString(&string, MetaString(&s->selected_unit_id));
	AppendCString(&string, "(Id<Unit>)\n");

	AppendCString(&string, "  selected_ability_id: ");
	AppendString(&string, MetaString(&s->selected_ability_id));
	AppendCString(&string, "(Id<Ability>)\n");

	AppendCString(&string, "  units: ");
	AppendString(&string, MetaString(&s->units));
	AppendCString(&string, "(Array<UnitId>)\n");

	AppendCString(&string, "  preview_intents: ");
	AppendString(&string, MetaString(&s->preview_intents));
	AppendCString(&string, "(Array<Intent>)\n");

	AppendCString(&string, "  preview_damage_timer: ");
	AppendString(&string, MetaString(&s->preview_damage_timer));
	AppendCString(&string, "(OscillatingTimer)\n");

	AppendCString(&string, "  end_player_turn_timer: ");
	AppendString(&string, MetaString(&s->end_player_turn_timer));
	AppendCString(&string, "(Timer)\n");

	AppendCString(&string, "  best_choice_string: ");
	AppendString(&string, MetaString(&s->best_choice_string));
	AppendCString(&string, "(String)\n");

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// better_text_parsing.h
// ------------------------------------------

// ---------------FILE START---------------
// bitmap.h
// ------------------------------------------

String MetaString(const BgraPixel *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "BgraPixel {\n");

	AppendCString(&string, "  b: %u (u8)\n", s->b);

	AppendCString(&string, "  g: %u (u8)\n", s->g);

	AppendCString(&string, "  r: %u (u8)\n", s->r);

	AppendCString(&string, "  a: %u (u8)\n", s->a);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const Bitmap *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Bitmap {\n");

	AppendCString(&string, "  width: %u (u32)\n", s->width);

	AppendCString(&string, "  height: %u (u32)\n", s->height);

	AppendCString(&string, "  pixels: %p (BgraPixel *)\n", s->pixels);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// buffer.h
// ------------------------------------------

String MetaString(const Buffer *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Buffer {\n");

	AppendCString(&string, "  data: %p (char *)\n", s->data);

	AppendCString(&string, "  p: %p (char *)\n", s->p);

	AppendCString(&string, "  byte_count: %zu (size_t)\n", s->byte_count);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// camera.h
// ------------------------------------------

String MetaString(const Camera *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Camera {\n");

	AppendCString(&string, "  pos: ");
	AppendString(&string, MetaString(&s->pos));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  view: ");
	AppendString(&string, MetaString(&s->view));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// campaign.h
// ------------------------------------------

String MetaString(const CampaignState *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "CampaignState::");
	switch(*s)
	{
		case(CampaignState::Invalid): {
			AppendCString(&string, "Invalid");
		} break;
		case(CampaignState::MapSelection): {
			AppendCString(&string, "MapSelection");
		} break;
		case(CampaignState::TransitionIntoMap): {
			AppendCString(&string, "TransitionIntoMap");
		} break;
		case(CampaignState::InMap): {
			AppendCString(&string, "InMap");
		} break;
		case(CampaignState::InRoom): {
			AppendCString(&string, "InRoom");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

String MetaString(const Campaign *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Campaign {\n");

	AppendCString(&string, "  state: ");
	AppendString(&string, MetaString(&s->state));
	AppendCString(&string, "(CampaignState)\n");

	AppendCString(&string, "  arena_id: ");
	AppendString(&string, MetaString(&s->arena_id));
	AppendCString(&string, "(PoolId<Arena>)\n");

	AppendCString(&string, "  battle_arena_id: ");
	AppendString(&string, MetaString(&s->battle_arena_id));
	AppendCString(&string, "(PoolId<Arena>)\n");

	AppendCString(&string, "  map_generation_work_queue: %p (WorkQueue *)\n", s->map_generation_work_queue);

	AppendCString(&string, "  show_generation: %p (bool[])\n", s->show_generation);

	AppendCString(&string, "  generation_finished: %p (bool[])\n", s->generation_finished);

	AppendCString(&string, "  restart_counts: %p (int[])\n", s->restart_counts);

	AppendCString(&string, "  max_speeds: %p (float[])\n", s->max_speeds);

	AppendCString(&string, "  maps: %p (NodeGraph[])\n", s->maps);

	AppendCString(&string, "  generation_params_template: ");
	AppendString(&string, MetaString(&s->generation_params_template));
	AppendCString(&string, "(GenerateNodeGraph_Params)\n");

	AppendCString(&string, "  selected_map_index: %d (int)\n", s->selected_map_index);

	AppendCString(&string, "  map_zoom_timer: ");
	AppendString(&string, MetaString(&s->map_zoom_timer));
	AppendCString(&string, "(Timer)\n");

	AppendCString(&string, "  start_camera: ");
	AppendString(&string, MetaString(&s->start_camera));
	AppendCString(&string, "(Camera)\n");

	AppendCString(&string, "  end_camera: ");
	AppendString(&string, MetaString(&s->end_camera));
	AppendCString(&string, "(Camera)\n");

	AppendCString(&string, "  start_node_pos: ");
	AppendString(&string, MetaString(&s->start_node_pos));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  node_pulse_timer: ");
	AppendString(&string, MetaString(&s->node_pulse_timer));
	AppendCString(&string, "(OscillatingTimer)\n");

	AppendCString(&string, "  room_is_init: %d (bool)\n", s->room_is_init);

	AppendCString(&string, "  rooms: ");
	AppendString(&string, MetaString(&s->rooms));
	AppendCString(&string, "(Array<Room>)\n");

	AppendCString(&string, "  current_room_index: %d (int)\n", s->current_room_index);

	AppendCString(&string, "  player_party: ");
	AppendString(&string, MetaString(&s->player_party));
	AppendCString(&string, "(UnitSet)\n");

	AppendCString(&string, "  map_camera: ");
	AppendString(&string, MetaString(&s->map_camera));
	AppendCString(&string, "(Camera)\n");

	AppendCString(&string, "  current_battle: ");
	AppendString(&string, MetaString(&s->current_battle));
	AppendCString(&string, "(Battle)\n");

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// color.h
// ------------------------------------------

String MetaString(const Color *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Color {\n");

	AppendCString(&string, "  r: %f (float)\n", s->r);

	AppendCString(&string, "  g: %f (float)\n", s->g);

	AppendCString(&string, "  b: %f (float)\n", s->b);

	AppendCString(&string, "  a: %f (float)\n", s->a);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// const.h
// ------------------------------------------




// ---------------FILE START---------------
// debug.h
// ------------------------------------------

String MetaString(const TimedBlockEntry *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "TimedBlockEntry {\n");

	AppendCString(&string, "  filename: %p (char *)\n", s->filename);

	AppendCString(&string, "  function_name: %p (char *)\n", s->function_name);

	AppendCString(&string, "  line_number: %d (int)\n", s->line_number);

	AppendCString(&string, "  hit_count: %u (u64)\n", s->hit_count);

	AppendCString(&string, "  total_cycle_count: %u (u64)\n", s->total_cycle_count);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const TimedBlock *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "TimedBlock {\n");

	AppendCString(&string, "  entry: %p (TimedBlockEntry *)\n", s->entry);

	AppendCString(&string, "  start_count: %u (u64)\n", s->start_count);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// debug_overlay.h
// ------------------------------------------

String MetaString(const OverlayOption *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "OverlayOption::");
	switch(*s)
	{
		case(OverlayOption::Arenas): {
			AppendCString(&string, "Arenas");
		} break;
		case(OverlayOption::TimedBlocks): {
			AppendCString(&string, "TimedBlocks");
		} break;
		case(OverlayOption::CpuFrametime): {
			AppendCString(&string, "CpuFrametime");
		} break;
		case(OverlayOption::GpuFrametime): {
			AppendCString(&string, "GpuFrametime");
		} break;
		case(OverlayOption::Tables): {
			AppendCString(&string, "Tables");
		} break;
		case(OverlayOption::COUNT): {
			AppendCString(&string, "COUNT");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

String MetaString(const DebugOverlay *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "DebugOverlay {\n");

	AppendCString(&string, "  option_active: %p (bool[])\n", s->option_active);

	AppendCString(&string, "  window_positions: %p (Vec2f[])\n", s->window_positions);

	AppendCString(&string, "  dragging_index: %d (int)\n", s->dragging_index);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// draw.h
// ------------------------------------------

String MetaString(const Texture *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Texture {\n");

	AppendCString(&string, "  id: %u (u32)\n", s->id);

	AppendCString(&string, "  width: %d (int)\n", s->width);

	AppendCString(&string, "  height: %d (int)\n", s->height);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const Framebuffer *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Framebuffer {\n");

	AppendCString(&string, "  id: %u (u32)\n", s->id);

	AppendCString(&string, "  width: %d (int)\n", s->width);

	AppendCString(&string, "  height: %d (int)\n", s->height);

	AppendCString(&string, "  texture: ");
	AppendString(&string, MetaString(&s->texture));
	AppendCString(&string, "(Texture)\n");

	AppendCString(&string, "  depth_rbo: %u (u32)\n", s->depth_rbo);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const DirectedLineLayout *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "DirectedLineLayout {\n");

	AppendCString(&string, "  segment_count: %d (int)\n", s->segment_count);

	AppendCString(&string, "  line_color: ");
	AppendString(&string, MetaString(&s->line_color));
	AppendCString(&string, "(Color)\n");

	AppendCString(&string, "  arrow_color: ");
	AppendString(&string, MetaString(&s->arrow_color));
	AppendCString(&string, "(Color)\n");

	AppendCString(&string, "  arrow_size: %f (float)\n", s->arrow_size);

	AppendCString(&string, "  arrow_angle: %f (float)\n", s->arrow_angle);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// editor.h
// ------------------------------------------

String MetaString(const EditorMode *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "EditorMode::");
	switch(*s)
	{
		case(EditorMode::None): {
			AppendCString(&string, "None");
		} break;
		case(EditorMode::Ability): {
			AppendCString(&string, "Ability");
		} break;
		case(EditorMode::Breed): {
			AppendCString(&string, "Breed");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

String MetaString(const InputElementType *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "InputElementType::");
	switch(*s)
	{
		case(InputElementType::None): {
			AppendCString(&string, "None");
		} break;
		case(InputElementType::String): {
			AppendCString(&string, "String");
		} break;
		case(InputElementType::Integer): {
			AppendCString(&string, "Integer");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

String MetaString(const AbilityPropertyIndex *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "AbilityPropertyIndex::");
	switch(*s)
	{
		case(AbilityPropertyIndex::search): {
			AppendCString(&string, "search");
		} break;
		case(AbilityPropertyIndex::name): {
			AppendCString(&string, "name");
		} break;
		case(AbilityPropertyIndex::tier0required_vigor): {
			AppendCString(&string, "tier0required_vigor");
		} break;
		case(AbilityPropertyIndex::tier0required_focus): {
			AppendCString(&string, "tier0required_focus");
		} break;
		case(AbilityPropertyIndex::tier0required_armor): {
			AppendCString(&string, "tier0required_armor");
		} break;
		case(AbilityPropertyIndex::tier1required_vigor): {
			AppendCString(&string, "tier1required_vigor");
		} break;
		case(AbilityPropertyIndex::tier1required_focus): {
			AppendCString(&string, "tier1required_focus");
		} break;
		case(AbilityPropertyIndex::tier1required_armor): {
			AppendCString(&string, "tier1required_armor");
		} break;
		case(AbilityPropertyIndex::tier2required_vigor): {
			AppendCString(&string, "tier2required_vigor");
		} break;
		case(AbilityPropertyIndex::tier2required_focus): {
			AppendCString(&string, "tier2required_focus");
		} break;
		case(AbilityPropertyIndex::tier2required_armor): {
			AppendCString(&string, "tier2required_armor");
		} break;
		case(AbilityPropertyIndex::COUNT): {
			AppendCString(&string, "COUNT");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

String MetaString(const BreedPropertyIndex *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "BreedPropertyIndex::");
	switch(*s)
	{
		case(BreedPropertyIndex::COUNT): {
			AppendCString(&string, "COUNT");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

String MetaString(const InputElement *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "InputElement {\n");

	AppendCString(&string, "  type: ");
	AppendString(&string, MetaString(&s->type));
	AppendCString(&string, "(InputElementType)\n");

	AppendCString(&string, "  label: ");
	AppendString(&string, MetaString(&s->label));
	AppendCString(&string, "(String)\n");

	AppendCString(&string, "  pos: ");
	AppendString(&string, MetaString(&s->pos));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  text: ");
	AppendString(&string, MetaString(&s->text));
	AppendCString(&string, "(String)\n");

	AppendCString(&string, "  value_ptr: %p (void *)\n", s->value_ptr);

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

	AppendCString(&string, "  arena_id: ");
	AppendString(&string, MetaString(&s->arena_id));
	AppendCString(&string, "(PoolId<Arena>)\n");

	AppendCString(&string, "  mode: ");
	AppendString(&string, MetaString(&s->mode));
	AppendCString(&string, "(EditorMode)\n");

	AppendCString(&string, "  search_panel_layout: ");
	AppendString(&string, MetaString(&s->search_panel_layout));
	AppendCString(&string, "(ListPanelLayout)\n");

	AppendCString(&string, "  panel_scroll_acc: %f (float)\n", s->panel_scroll_acc);

	AppendCString(&string, "  panel_scroll_vel: %f (float)\n", s->panel_scroll_vel);

	AppendCString(&string, "  panel_scroll_pos: %f (float)\n", s->panel_scroll_pos);

	AppendCString(&string, "  panel_scroll_friction: %f (float)\n", s->panel_scroll_friction);

	AppendCString(&string, "  panel_scroll_velocity_minimum: %f (float)\n", s->panel_scroll_velocity_minimum);

	AppendCString(&string, "  active_index: %d (int)\n", s->active_index);

	AppendCString(&string, "  text_cursor_pos: %d (int)\n", s->text_cursor_pos);

	AppendCString(&string, "  input_elements: %p (InputElement[])\n", s->input_elements);

	AppendCString(&string, "  temp_ability: ");
	AppendString(&string, MetaString(&s->temp_ability));
	AppendCString(&string, "(Ability)\n");

	AppendCString(&string, "  temp_ability_id: ");
	AppendString(&string, MetaString(&s->temp_ability_id));
	AppendCString(&string, "(Id<Ability>)\n");

	AppendCString(&string, "  temp_breed: ");
	AppendString(&string, MetaString(&s->temp_breed));
	AppendCString(&string, "(Breed)\n");

	AppendCString(&string, "  temp_breed_id: ");
	AppendString(&string, MetaString(&s->temp_breed_id));
	AppendCString(&string, "(Id<Breed>)\n");

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// effect.h
// ------------------------------------------

String MetaString(const EffectType *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "EffectType::");
	switch(*s)
	{
		case(EffectType::NoEffect): {
			AppendCString(&string, "NoEffect");
		} break;
		case(EffectType::Damage): {
			AppendCString(&string, "Damage");
		} break;
		case(EffectType::DamageIgnoreArmor): {
			AppendCString(&string, "DamageIgnoreArmor");
		} break;
		case(EffectType::Restore): {
			AppendCString(&string, "Restore");
		} break;
		case(EffectType::Gift): {
			AppendCString(&string, "Gift");
		} break;
		case(EffectType::Steal): {
			AppendCString(&string, "Steal");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

String MetaString(const Effect *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Effect {\n");

	AppendCString(&string, "  type: ");
	AppendString(&string, MetaString(&s->type));
	AppendCString(&string, "(EffectType)\n");

	AppendCString(&string, "  params: %p (void *)\n", s->params);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const EffectParams_Damage *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "EffectParams_Damage {\n");

	AppendCString(&string, "  amount: ");
	AppendString(&string, MetaString(&s->amount));
	AppendCString(&string, "(TraitSet)\n");

	AppendCString(&string, "}");

	return string;
}

String MetaString(const EffectParams_DamageIgnoreArmor *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "EffectParams_DamageIgnoreArmor {\n");

	AppendCString(&string, "  amount: ");
	AppendString(&string, MetaString(&s->amount));
	AppendCString(&string, "(TraitSet)\n");

	AppendCString(&string, "}");

	return string;
}

String MetaString(const EffectParams_Restore *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "EffectParams_Restore {\n");

	AppendCString(&string, "  amount: ");
	AppendString(&string, MetaString(&s->amount));
	AppendCString(&string, "(TraitSet)\n");

	AppendCString(&string, "}");

	return string;
}

String MetaString(const EffectParams_Gift *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "EffectParams_Gift {\n");

	AppendCString(&string, "  amount: ");
	AppendString(&string, MetaString(&s->amount));
	AppendCString(&string, "(TraitSet)\n");

	AppendCString(&string, "}");

	return string;
}

String MetaString(const EffectParams_Steal *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "EffectParams_Steal {\n");

	AppendCString(&string, "  amount: ");
	AppendString(&string, MetaString(&s->amount));
	AppendCString(&string, "(TraitSet)\n");

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// enemy_ai.h
// ------------------------------------------

String MetaString(const BattleScore *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "BattleScore {\n");

	AppendCString(&string, "  total: %f (float)\n", s->total);

	AppendCString(&string, "  rel_change: %f (float)\n", s->rel_change);

	AppendCString(&string, "  abs_change: %f (float)\n", s->abs_change);

	AppendCString(&string, "  ally_potential: %f (float)\n", s->ally_potential);

	AppendCString(&string, "  enemy_potential: %f (float)\n", s->enemy_potential);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const AiIntent *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "AiIntent {\n");

	AppendCString(&string, "  ability: ");
	AppendString(&string, MetaString(&s->ability));
	AppendCString(&string, "(Ability)\n");

	AppendCString(&string, "  ability_id: ");
	AppendString(&string, MetaString(&s->ability_id));
	AppendCString(&string, "(AbilityId)\n");

	AppendCString(&string, "  caster_index: %u (u8)\n", s->caster_index);

	AppendCString(&string, "  target_count: %u (u8)\n", s->target_count);

	AppendCString(&string, "  unit_indices: %p (u8[])\n", s->unit_indices);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// fixed_array.h
// ------------------------------------------

// ---------------FILE START---------------
// font_loading.h
// ------------------------------------------

// ---------------FILE START---------------
// frametimes_draw.h
// ------------------------------------------

String MetaString(const FrametimeGraphState *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "FrametimeGraphState {\n");

	AppendCString(&string, "  frametimes: %p (float *)\n", s->frametimes);

	AppendCString(&string, "  cur_frametime_index: %d (int)\n", s->cur_frametime_index);

	AppendCString(&string, "  entry_count: %d (int)\n", s->entry_count);

	AppendCString(&string, "  graph_max: %f (float)\n", s->graph_max);

	AppendCString(&string, "  ui_container: ");
	AppendString(&string, MetaString(&s->ui_container));
	AppendCString(&string, "(ImguiContainer)\n");

	AppendCString(&string, "  label_text: ");
	AppendString(&string, MetaString(&s->label_text));
	AppendCString(&string, "(TextLayout)\n");

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// freetype.h
// ------------------------------------------

// ---------------FILE START---------------
// freetype_wrapper.h
// ------------------------------------------

// ---------------FILE START---------------
// game.h
// ------------------------------------------

String MetaString(const GameInitData *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "GameInitData {\n");

	AppendCString(&string, "  per_frame_arena_id: ");
	AppendString(&string, MetaString(&s->per_frame_arena_id));
	AppendCString(&string, "(PoolId<Arena>)\n");

	AppendCString(&string, "  permanent_arena_id: ");
	AppendString(&string, MetaString(&s->permanent_arena_id));
	AppendCString(&string, "(PoolId<Arena>)\n");

	AppendCString(&string, "  arena_pool_mutex_handle: %p (void *)\n", s->arena_pool_mutex_handle);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// game_state.h
// ------------------------------------------

String MetaString(const GameState *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "GameState::");
	switch(*s)
	{
		case(GameState::None): {
			AppendCString(&string, "None");
		} break;
		case(GameState::Quit): {
			AppendCString(&string, "Quit");
		} break;
		case(GameState::MainMenu): {
			AppendCString(&string, "MainMenu");
		} break;
		case(GameState::Campaign): {
			AppendCString(&string, "Campaign");
		} break;
		case(GameState::Editor): {
			AppendCString(&string, "Editor");
		} break;
		case(GameState::Options): {
			AppendCString(&string, "Options");
		} break;
		case(GameState::AiExplorer): {
			AppendCString(&string, "AiExplorer");
		} break;
		case(GameState::Test): {
			AppendCString(&string, "Test");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

// ---------------FILE START---------------
// generate_node_graph_params.h
// ------------------------------------------



String MetaString(const GenerateNodeGraph_Params *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "GenerateNodeGraph_Params {\n");

	AppendCString(&string, "  thread_finished: %p (bool *)\n", s->thread_finished);

	AppendCString(&string, "  restart_count: %p (int *)\n", s->restart_count);

	AppendCString(&string, "  max_speed: %p (float *)\n", s->max_speed);

	AppendCString(&string, "  graph: %p (NodeGraph *)\n", s->graph);

	AppendCString(&string, "  main_path_min: %u (u32)\n", s->main_path_min);

	AppendCString(&string, "  main_path_max: %u (u32)\n", s->main_path_max);

	AppendCString(&string, "  max_linear_branch_length: %d (int)\n", s->max_linear_branch_length);

	AppendCString(&string, "  linear_branch_extension_chance: %f (float)\n", s->linear_branch_extension_chance);

	AppendCString(&string, "  loop_generation_count: %d (int)\n", s->loop_generation_count);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// geometry.h
// ------------------------------------------

// ---------------FILE START---------------
// global.h
// ------------------------------------------


// ---------------FILE START---------------
// image.h
// ------------------------------------------

// ---------------FILE START---------------
// imgui.h
// ------------------------------------------

String MetaString(const ListPanelLayout *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "ListPanelLayout {\n");

	AppendCString(&string, "  rect: ");
	AppendString(&string, MetaString(&s->rect));
	AppendCString(&string, "(Rect)\n");

	AppendCString(&string, "}");

	return string;
}

String MetaString(const ListPanelResponse *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "ListPanelResponse {\n");

	AppendCString(&string, "  hovered_index: %d (int)\n", s->hovered_index);

	AppendCString(&string, "  pressed_index: %d (int)\n", s->pressed_index);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const ListPanel_ *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "ListPanel_ {\n");

	AppendCString(&string, "  layout: ");
	AppendString(&string, MetaString(&s->layout));
	AppendCString(&string, "(ListPanelLayout)\n");

	AppendCString(&string, "  cur_entry_count: %d (int)\n", s->cur_entry_count);

	AppendCString(&string, "  scroll_offset: %f (float)\n", s->scroll_offset);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const ButtonLayout *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "ButtonLayout {\n");

	AppendCString(&string, "  label_layout: ");
	AppendString(&string, MetaString(&s->label_layout));
	AppendCString(&string, "(TextLayout)\n");

	AppendCString(&string, "  button_color: ");
	AppendString(&string, MetaString(&s->button_color));
	AppendCString(&string, "(Color)\n");

	AppendCString(&string, "  button_hover_color: ");
	AppendString(&string, MetaString(&s->button_hover_color));
	AppendCString(&string, "(Color)\n");

	AppendCString(&string, "  label_hover_color: ");
	AppendString(&string, MetaString(&s->label_hover_color));
	AppendCString(&string, "(Color)\n");

	AppendCString(&string, "  align: ");
	AppendString(&string, MetaString(&s->align));
	AppendCString(&string, "(Align)\n");

	AppendCString(&string, "  is_ui: %d (bool)\n", s->is_ui);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const TextEntryLayout *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "TextEntryLayout {\n");

	AppendCString(&string, "  size: ");
	AppendString(&string, MetaString(&s->size));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  label_layout: ");
	AppendString(&string, MetaString(&s->label_layout));
	AppendCString(&string, "(TextLayout)\n");

	AppendCString(&string, "  text_layout: ");
	AppendString(&string, MetaString(&s->text_layout));
	AppendCString(&string, "(TextLayout)\n");

	AppendCString(&string, "  align: ");
	AppendString(&string, MetaString(&s->align));
	AppendCString(&string, "(Align)\n");

	AppendCString(&string, "}");

	return string;
}

String MetaString(const IntegerBoxLayout *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "IntegerBoxLayout {\n");

	AppendCString(&string, "  size: ");
	AppendString(&string, MetaString(&s->size));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  border_color: ");
	AppendString(&string, MetaString(&s->border_color));
	AppendCString(&string, "(Color)\n");

	AppendCString(&string, "  label_layout: ");
	AppendString(&string, MetaString(&s->label_layout));
	AppendCString(&string, "(TextLayout)\n");

	AppendCString(&string, "  text_layout: ");
	AppendString(&string, MetaString(&s->text_layout));
	AppendCString(&string, "(TextLayout)\n");

	AppendCString(&string, "}");

	return string;
}

String MetaString(const IntegerBoxResponse *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "IntegerBoxResponse {\n");

	AppendCString(&string, "  value_change: %d (int)\n", s->value_change);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const ImguiContainer *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "ImguiContainer {\n");

	AppendCString(&string, "  pos: ");
	AppendString(&string, MetaString(&s->pos));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  max_size: ");
	AppendString(&string, MetaString(&s->max_size));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  pen: ");
	AppendString(&string, MetaString(&s->pen));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  at_row_start: %d (bool)\n", s->at_row_start);

	AppendCString(&string, "  cur_row_count: %d (int)\n", s->cur_row_count);

	AppendCString(&string, "  button_layout: ");
	AppendString(&string, MetaString(&s->button_layout));
	AppendCString(&string, "(ButtonLayout)\n");

	AppendCString(&string, "  text_entry_layout: ");
	AppendString(&string, MetaString(&s->text_entry_layout));
	AppendCString(&string, "(TextEntryLayout)\n");

	AppendCString(&string, "}");

	return string;
}

String MetaString(const ButtonResponse *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "ButtonResponse {\n");

	AppendCString(&string, "  pressed: %d (bool)\n", s->pressed);

	AppendCString(&string, "  hovered: %d (bool)\n", s->hovered);

	AppendCString(&string, "  just_now_hovered: %d (bool)\n", s->just_now_hovered);

	AppendCString(&string, "  rect: ");
	AppendString(&string, MetaString(&s->rect));
	AppendCString(&string, "(Rect)\n");

	AppendCString(&string, "}");

	return string;
}

String MetaString(const TextEntryResponse *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "TextEntryResponse {\n");

	AppendCString(&string, "  pressed: %d (bool)\n", s->pressed);

	AppendCString(&string, "  hovered: %d (bool)\n", s->hovered);

	AppendCString(&string, "  clicked_cursor_pos: %d (int)\n", s->clicked_cursor_pos);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// input.h
// ------------------------------------------

String MetaString(const InputState *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "InputState {\n");

	AppendCString(&string, "  pressed_keys: %p (u8[])\n", s->pressed_keys);

	AppendCString(&string, "  released_keys: %p (u8[])\n", s->released_keys);

	AppendCString(&string, "  repeated_keys: %p (u8[])\n", s->repeated_keys);

	AppendCString(&string, "  down_keys: %p (u8[])\n", s->down_keys);

	AppendCString(&string, "  any_key: %u (u8)\n", s->any_key);

	AppendCString(&string, "  prev_mouse_pos: ");
	AppendString(&string, MetaString(&s->prev_mouse_pos));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  mouse_pos: ");
	AppendString(&string, MetaString(&s->mouse_pos));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  mouse_scroll: %d (int)\n", s->mouse_scroll);

	AppendCString(&string, "  mouse_focus_taken: %d (bool)\n", s->mouse_focus_taken);

	AppendCString(&string, "  utf32_translated_stream: %p (u32 *)\n", s->utf32_translated_stream);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// input_vk_constants.h
// ------------------------------------------

String MetaString(const VirtualKey *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "VirtualKey::");
	switch(*s)
	{
		case(VirtualKey::None): {
			AppendCString(&string, "None");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

// ---------------FILE START---------------
// intent.h
// ------------------------------------------

String MetaString(const Intent *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Intent {\n");

	AppendCString(&string, "  caster_id: ");
	AppendString(&string, MetaString(&s->caster_id));
	AppendCString(&string, "(Id<Unit>)\n");

	AppendCString(&string, "  ability_id: ");
	AppendString(&string, MetaString(&s->ability_id));
	AppendCString(&string, "(Id<Ability>)\n");

	AppendCString(&string, "  target_set: ");
	AppendString(&string, MetaString(&s->target_set));
	AppendCString(&string, "(Array<UnitId>)\n");

	AppendCString(&string, "  position: %u (u8)\n", s->position);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// keybinds.h
// ------------------------------------------

String MetaString(const KeyBind *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "KeyBind::");
	switch(*s)
	{
		case(KeyBind::Select): {
			AppendCString(&string, "Select");
		} break;
		case(KeyBind::Deselect): {
			AppendCString(&string, "Deselect");
		} break;
		case(KeyBind::Exit): {
			AppendCString(&string, "Exit");
		} break;
		case(KeyBind::SelectUnit1): {
			AppendCString(&string, "SelectUnit1");
		} break;
		case(KeyBind::SelectUnit2): {
			AppendCString(&string, "SelectUnit2");
		} break;
		case(KeyBind::SelectUnit3): {
			AppendCString(&string, "SelectUnit3");
		} break;
		case(KeyBind::SelectUnit4): {
			AppendCString(&string, "SelectUnit4");
		} break;
		case(KeyBind::CycleUnits): {
			AppendCString(&string, "CycleUnits");
		} break;
		case(KeyBind::SelectAbility1): {
			AppendCString(&string, "SelectAbility1");
		} break;
		case(KeyBind::SelectAbility2): {
			AppendCString(&string, "SelectAbility2");
		} break;
		case(KeyBind::SelectAbility3): {
			AppendCString(&string, "SelectAbility3");
		} break;
		case(KeyBind::SelectAbility4): {
			AppendCString(&string, "SelectAbility4");
		} break;
		case(KeyBind::COUNT): {
			AppendCString(&string, "COUNT");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

// ---------------FILE START---------------
// lang.h
// ------------------------------------------

// ---------------FILE START---------------
// log.h
// ------------------------------------------

String MetaString(const LogState *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "LogState {\n");

	AppendCString(&string, "  log_start_index: %u (u8)\n", s->log_start_index);

	AppendCString(&string, "  log_cur_index: %u (u8)\n", s->log_cur_index);

	AppendCString(&string, "  log_length: %u (u8)\n", s->log_length);

	AppendCString(&string, "  log_strings: %p (char[][])\n", s->log_strings);

	AppendCString(&string, "  queue: %p (WorkQueue *)\n", s->queue);

	AppendCString(&string, "  show_log: %d (bool)\n", s->show_log);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// macros.h
// ------------------------------------------

// ---------------FILE START---------------
// main_menu.h
// ------------------------------------------

String MetaString(const MainMenu *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "MainMenu {\n");

	AppendCString(&string, "  option_strings: ");
	AppendString(&string, MetaString(&s->option_strings));
	AppendCString(&string, "(Array<String>)\n");

	AppendCString(&string, "  selected_option: %d (int)\n", s->selected_option);

	AppendCString(&string, "  hovered_option: %d (int)\n", s->hovered_option);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// map.h
// ------------------------------------------

String MetaString(const MapResponse *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "MapResponse {\n");

	AppendCString(&string, "  start_node_pos: ");
	AppendString(&string, MetaString(&s->start_node_pos));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  hovered_node_index: %d (int)\n", s->hovered_node_index);

	AppendCString(&string, "  newly_hovered: %d (bool)\n", s->newly_hovered);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// math.h
// ------------------------------------------

// ---------------FILE START---------------
// memory.h
// ------------------------------------------

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

	AppendCString(&string, "  allocs_since_reset: %d (int)\n", s->allocs_since_reset);

	AppendCString(&string, "  debug_name: %p (char[])\n", s->debug_name);

	AppendCString(&string, "  max_current: %p (void *)\n", s->max_current);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// memory_draw.h
// ------------------------------------------

// ---------------FILE START---------------
// meta.h
// ------------------------------------------

// ---------------FILE START---------------
// node_graph.h
// ------------------------------------------

String MetaString(const Node *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Node {\n");

	AppendCString(&string, "  pos: ");
	AppendString(&string, MetaString(&s->pos));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  vel: ");
	AppendString(&string, MetaString(&s->vel));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  completed: %d (bool)\n", s->completed);

	AppendCString(&string, "  reachable: %d (bool)\n", s->reachable);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const Edge *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Edge {\n");

	AppendCString(&string, "  a: %d (int)\n", s->a);

	AppendCString(&string, "  b: %d (int)\n", s->b);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const NodeGraph *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "NodeGraph {\n");

	AppendCString(&string, "  nodes: ");
	AppendString(&string, MetaString(&s->nodes));
	AppendCString(&string, "(Array<Node>)\n");

	AppendCString(&string, "  edges: ");
	AppendString(&string, MetaString(&s->edges));
	AppendCString(&string, "(Array<Edge>)\n");

	AppendCString(&string, "  start_index: %d (int)\n", s->start_index);

	AppendCString(&string, "  end_index: %d (int)\n", s->end_index);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const NodeGraphResponse *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "NodeGraphResponse {\n");

	AppendCString(&string, "  start_node_pos: ");
	AppendString(&string, MetaString(&s->start_node_pos));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  newly_hovered: %d (bool)\n", s->newly_hovered);

	AppendCString(&string, "  hovered_node_index: %d (int)\n", s->hovered_node_index);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const ForceSimParams *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "ForceSimParams {\n");

	AppendCString(&string, "  temp_arena: ");
	AppendString(&string, MetaString(&s->temp_arena));
	AppendCString(&string, "(PoolId<Arena>)\n");

	AppendCString(&string, "  edge_free_length: %f (float)\n", s->edge_free_length);

	AppendCString(&string, "  spring_constant: %f (float)\n", s->spring_constant);

	AppendCString(&string, "  charge_strength: %f (float)\n", s->charge_strength);

	AppendCString(&string, "  friction: %f (float)\n", s->friction);

	AppendCString(&string, "  max_speed: %f (float)\n", s->max_speed);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const ForceSimInvalidationReason *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "ForceSimInvalidationReason::");
	switch(*s)
	{
		case(ForceSimInvalidationReason::Unspecified): {
			AppendCString(&string, "Unspecified");
		} break;
		case(ForceSimInvalidationReason::NotFullyConnected): {
			AppendCString(&string, "NotFullyConnected");
		} break;
		case(ForceSimInvalidationReason::MaxSpeedExceeded): {
			AppendCString(&string, "MaxSpeedExceeded");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

String MetaString(const ForceSimState *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "ForceSimState {\n");

	AppendCString(&string, "  invalid_state: %d (bool)\n", s->invalid_state);

	AppendCString(&string, "  invalidation_reason: ");
	AppendString(&string, MetaString(&s->invalidation_reason));
	AppendCString(&string, "(ForceSimInvalidationReason)\n");

	AppendCString(&string, "  semi_stable: %d (bool)\n", s->semi_stable);

	AppendCString(&string, "  max_speed: %f (float)\n", s->max_speed);

	AppendCString(&string, "  finished: %d (bool)\n", s->finished);

	AppendCString(&string, "  restart_count: %d (int)\n", s->restart_count);

	AppendCString(&string, "  max_restart_count: %d (int)\n", s->max_restart_count);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// options_menu.h
// ------------------------------------------

String MetaString(const OptionsMenu *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "OptionsMenu {\n");

	AppendCString(&string, "  selected_option: %d (int)\n", s->selected_option);

	AppendCString(&string, "  option_being_modified: %d (bool)\n", s->option_being_modified);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// oscillating_timer.h
// ------------------------------------------

String MetaString(const OscillatingTimer *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "OscillatingTimer {\n");

	AppendCString(&string, "  cur: %f (float)\n", s->cur);

	AppendCString(&string, "  low: %f (float)\n", s->low);

	AppendCString(&string, "  high: %f (float)\n", s->high);

	AppendCString(&string, "  decreasing: %d (bool)\n", s->decreasing);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// passive_skill_tree.h
// ------------------------------------------

String MetaString(const PassiveSkill *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "PassiveSkill {\n");

	AppendCString(&string, "  name: %p (char[])\n", s->name);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const PassiveNode *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "PassiveNode {\n");

	AppendCString(&string, "  passive_skill: %p (PassiveSkill *)\n", s->passive_skill);

	AppendCString(&string, "  children: %p (PassiveNode[])\n", s->children);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const PassiveSkillTree *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "PassiveSkillTree {\n");

	AppendCString(&string, "  root: %p (PassiveNode *)\n", s->root);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// permutation.h
// ------------------------------------------

// ---------------FILE START---------------
// pool.h
// ------------------------------------------

template<typename Type>
String MetaString(const PoolId<Type> *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "PoolId {\n");

	AppendCString(&string, "  value: %d (int)\n", s->value);

	AppendCString(&string, "}");

	return string;
}

template<typename Type>
String MetaString(const PoolEntry<Type> *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "PoolEntry {\n");

	AppendCString(&string, "  data: ");
	AppendString(&string, MetaString(&s->data));
	AppendCString(&string, "(Type)\n");

	AppendCString(&string, "  id: ");
	AppendString(&string, MetaString(&s->id));
	AppendCString(&string, "(PoolId<Type>)\n");

	AppendCString(&string, "}");

	return string;
}

template<typename Type>
String MetaString(const Pool<Type> *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Pool {\n");

	AppendCString(&string, "  entries: %p (PoolEntry<Type> *)\n", s->entries);

	AppendCString(&string, "  entry_count: %d (int)\n", s->entry_count);

	AppendCString(&string, "  max_entry_count: %d (int)\n", s->max_entry_count);

	AppendCString(&string, "  id_counter: %d (int)\n", s->id_counter);

	AppendCString(&string, "  Pool: ");
	AppendString(&string, MetaString(&s->Pool));
	AppendCString(&string, "(template<typename Type>)\n");

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// random.h
// ------------------------------------------

String MetaString(const LCG *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "LCG {\n");

	AppendCString(&string, "  m: %u (u32)\n", s->m);

	AppendCString(&string, "  a: %u (u32)\n", s->a);

	AppendCString(&string, "  c: %u (u32)\n", s->c);

	AppendCString(&string, "  seed: %u (u32)\n", s->seed);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// ring_buffer.h
// ------------------------------------------

String MetaString(const RingBuffer *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "RingBuffer {\n");

	AppendCString(&string, "  next_index_to_read: %zu (size_t)\n", s->next_index_to_read);

	AppendCString(&string, "  next_index_to_write: %zu (size_t)\n", s->next_index_to_write);

	AppendCString(&string, "  size_in_bytes: %zu (size_t)\n", s->size_in_bytes);

	AppendCString(&string, "  data: %p (void *)\n", s->data);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// room.h
// ------------------------------------------

String MetaString(const RoomType *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "RoomType::");
	switch(*s)
	{
		case(RoomType::Undefined): {
			AppendCString(&string, "Undefined");
		} break;
		case(RoomType::Boss): {
			AppendCString(&string, "Boss");
		} break;
		case(RoomType::Battle): {
			AppendCString(&string, "Battle");
		} break;
		case(RoomType::Shop): {
			AppendCString(&string, "Shop");
		} break;
		case(RoomType::Camp): {
			AppendCString(&string, "Camp");
		} break;
		case(RoomType::Fishing): {
			AppendCString(&string, "Fishing");
		} break;
		case(RoomType::COUNT): {
			AppendCString(&string, "COUNT");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

String MetaString(const Room *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Room {\n");

	AppendCString(&string, "  type: ");
	AppendString(&string, MetaString(&s->type));
	AppendCString(&string, "(RoomType)\n");

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// sprite.h
// ------------------------------------------

String MetaString(const Sprite *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Sprite {\n");

	AppendCString(&string, "  texture: %u (GLuint)\n", s->texture);

	AppendCString(&string, "  size: ");
	AppendString(&string, MetaString(&s->size));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  origin: ");
	AppendString(&string, MetaString(&s->origin));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// string.h
// ------------------------------------------

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

// ---------------FILE START---------------
// table.h
// ------------------------------------------

template<typename Type>
String MetaString(const Id<Type> *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Id {\n");

	AppendCString(&string, "  index: %d (int)\n", s->index);

	AppendCString(&string, "  generation: %d (int)\n", s->generation);

	AppendCString(&string, "}");

	return string;
}




template<typename Type>
String MetaString(const TableEntry<Type> *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "TableEntry {\n");

	AppendCString(&string, "  data: ");
	AppendString(&string, MetaString(&s->data));
	AppendCString(&string, "(Type)\n");

	AppendCString(&string, "  active: %d (bool)\n", s->active);

	AppendCString(&string, "  id: ");
	AppendString(&string, MetaString(&s->id));
	AppendCString(&string, "(Id<Type>)\n");

	AppendCString(&string, "}");

	return string;
}

template<typename Type>
String MetaString(const Table<Type> *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Table {\n");

	AppendCString(&string, "  arena_id: ");
	AppendString(&string, MetaString(&s->arena_id));
	AppendCString(&string, "(PoolId<Arena>)\n");

	AppendCString(&string, "  entries: %p (TableEntry<Type> *)\n", s->entries);

	AppendCString(&string, "  entry_count: %d (int)\n", s->entry_count);

	AppendCString(&string, "  max_entry_count: %d (int)\n", s->max_entry_count);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// table_draw.h
// ------------------------------------------

String MetaString(const TableDrawMode *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "TableDrawMode::");
	switch(*s)
	{
		case(TableDrawMode::Ability): {
			AppendCString(&string, "Ability");
		} break;
		case(TableDrawMode::Breed): {
			AppendCString(&string, "Breed");
		} break;
		case(TableDrawMode::Unit): {
			AppendCString(&string, "Unit");
		} break;
		case(TableDrawMode::TempUnit): {
			AppendCString(&string, "TempUnit");
		} break;
		case(TableDrawMode::COUNT): {
			AppendCString(&string, "COUNT");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

String MetaString(const TableDrawState *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "TableDrawState {\n");

	AppendCString(&string, "  cur_mode: ");
	AppendString(&string, MetaString(&s->cur_mode));
	AppendCString(&string, "(TableDrawMode)\n");

	AppendCString(&string, "  ui_container: ");
	AppendString(&string, MetaString(&s->ui_container));
	AppendCString(&string, "(ImguiContainer)\n");

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// target_class.h
// ------------------------------------------

String MetaString(const TargetClass *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "TargetClass::");
	switch(*s)
	{
		case(TargetClass::none): {
			AppendCString(&string, "none");
		} break;
		case(TargetClass::self): {
			AppendCString(&string, "self");
		} break;
		case(TargetClass::single_ally): {
			AppendCString(&string, "single_ally");
		} break;
		case(TargetClass::single_ally_not_self): {
			AppendCString(&string, "single_ally_not_self");
		} break;
		case(TargetClass::all_allies): {
			AppendCString(&string, "all_allies");
		} break;
		case(TargetClass::all_allies_not_self): {
			AppendCString(&string, "all_allies_not_self");
		} break;
		case(TargetClass::single_enemy): {
			AppendCString(&string, "single_enemy");
		} break;
		case(TargetClass::all_enemies): {
			AppendCString(&string, "all_enemies");
		} break;
		case(TargetClass::single_unit): {
			AppendCString(&string, "single_unit");
		} break;
		case(TargetClass::single_unit_not_self): {
			AppendCString(&string, "single_unit_not_self");
		} break;
		case(TargetClass::all_units): {
			AppendCString(&string, "all_units");
		} break;
		case(TargetClass::COUNT): {
			AppendCString(&string, "COUNT");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

// ---------------FILE START---------------
// test_mode.h
// ------------------------------------------

String MetaString(const TestMode *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "TestMode {\n");

	AppendCString(&string, "  a0: ");
	AppendString(&string, MetaString(&s->a0));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  a1: ");
	AppendString(&string, MetaString(&s->a1));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  n: %d (int)\n", s->n);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// text_parsing.h
// ------------------------------------------

String MetaString(const TokenType_ *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "TokenType_::");
	switch(*s)
	{
		case(TokenType_::Unknown): {
			AppendCString(&string, "Unknown");
		} break;
		case(TokenType_::Pound): {
			AppendCString(&string, "Pound");
		} break;
		case(TokenType_::OpenSquigglyBracket): {
			AppendCString(&string, "OpenSquigglyBracket");
		} break;
		case(TokenType_::CloseSquigglyBracket): {
			AppendCString(&string, "CloseSquigglyBracket");
		} break;
		case(TokenType_::OpenSquareBracket): {
			AppendCString(&string, "OpenSquareBracket");
		} break;
		case(TokenType_::CloseSquareBracket): {
			AppendCString(&string, "CloseSquareBracket");
		} break;
		case(TokenType_::OpenAngleBracket): {
			AppendCString(&string, "OpenAngleBracket");
		} break;
		case(TokenType_::CloseAngleBracket): {
			AppendCString(&string, "CloseAngleBracket");
		} break;
		case(TokenType_::OpenParen): {
			AppendCString(&string, "OpenParen");
		} break;
		case(TokenType_::CloseParen): {
			AppendCString(&string, "CloseParen");
		} break;
		case(TokenType_::Colon): {
			AppendCString(&string, "Colon");
		} break;
		case(TokenType_::SemiColon): {
			AppendCString(&string, "SemiColon");
		} break;
		case(TokenType_::Asterisk): {
			AppendCString(&string, "Asterisk");
		} break;
		case(TokenType_::Comma): {
			AppendCString(&string, "Comma");
		} break;
		case(TokenType_::Tilde): {
			AppendCString(&string, "Tilde");
		} break;
		case(TokenType_::Equals): {
			AppendCString(&string, "Equals");
		} break;
		case(TokenType_::Backtick): {
			AppendCString(&string, "Backtick");
		} break;
		case(TokenType_::Identifier): {
			AppendCString(&string, "Identifier");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

String MetaString(const Token *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Token {\n");

	AppendCString(&string, "  type: ");
	AppendString(&string, MetaString(&s->type));
	AppendCString(&string, "(TokenType_)\n");

	AppendCString(&string, "  start: %p (char *)\n", s->start);

	AppendCString(&string, "  length: %zu (size_t)\n", s->length);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// text_render.h
// ------------------------------------------

String MetaString(const Font *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Font {\n");

	AppendCString(&string, "  is_init: %d (bool)\n", s->is_init);

	AppendCString(&string, "  base_size: %d (int)\n", s->base_size);

	AppendCString(&string, "  ascender: %d (int)\n", s->ascender);

	AppendCString(&string, "  height: %d (int)\n", s->height);

	AppendCString(&string, "  advance_x: %p (int *)\n", s->advance_x);

	AppendCString(&string, "  bitmap_left: %p (int *)\n", s->bitmap_left);

	AppendCString(&string, "  bitmap_top: %p (int *)\n", s->bitmap_top);

	AppendCString(&string, "  gl_texture: %p (GLuint *)\n", s->gl_texture);

	AppendCString(&string, "  texture_size: %p (Vec2i *)\n", s->texture_size);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const TextLayout *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "TextLayout {\n");

	AppendCString(&string, "  font: %p (Font *)\n", s->font);

	AppendCString(&string, "  color: ");
	AppendString(&string, MetaString(&s->color));
	AppendCString(&string, "(Color)\n");

	AppendCString(&string, "  hover_color: ");
	AppendString(&string, MetaString(&s->hover_color));
	AppendCString(&string, "(Color)\n");

	AppendCString(&string, "  has_hover_color: %d (bool)\n", s->has_hover_color);

	AppendCString(&string, "  font_size: %d (int)\n", s->font_size);

	AppendCString(&string, "  align: ");
	AppendString(&string, MetaString(&s->align));
	AppendCString(&string, "(Align)\n");

	AppendCString(&string, "  draw_debug: %d (bool)\n", s->draw_debug);

	AppendCString(&string, "  max_width: %f (float)\n", s->max_width);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const TextResponse *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "TextResponse {\n");

	AppendCString(&string, "  rect: ");
	AppendString(&string, MetaString(&s->rect));
	AppendCString(&string, "(Rect)\n");

	AppendCString(&string, "  hovered: %d (bool)\n", s->hovered);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// timer.h
// ------------------------------------------

String MetaString(const Timer *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Timer {\n");

	AppendCString(&string, "  start: %f (float)\n", s->start);

	AppendCString(&string, "  cur: %f (float)\n", s->cur);

	AppendCString(&string, "  length_s: %f (float)\n", s->length_s);

	AppendCString(&string, "  finished: %d (bool)\n", s->finished);

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// traitset.h
// ------------------------------------------

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

// ---------------FILE START---------------
// types.h
// ------------------------------------------

// ---------------FILE START---------------
// unit.h
// ------------------------------------------

String MetaString(const Team *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Team::");
	switch(*s)
	{
		case(Team::allies): {
			AppendCString(&string, "allies");
		} break;
		case(Team::enemies): {
			AppendCString(&string, "enemies");
		} break;
		default: {
			AppendCString(&string, "?????");
		} break;
	}

	return string;
}

String MetaString(const Breed *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Breed {\n");

	AppendCString(&string, "  init: %d (bool)\n", s->init);

	AppendCString(&string, "  name: ");
	AppendString(&string, MetaString(&s->name));
	AppendCString(&string, "(String)\n");

	AppendCString(&string, "  max_traits: ");
	AppendString(&string, MetaString(&s->max_traits));
	AppendCString(&string, "(TraitSet)\n");

	AppendCString(&string, "  ability_ids: %p (Id<Ability>[])\n", s->ability_ids);

	AppendCString(&string, "  tier: %d (int)\n", s->tier);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const Unit *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Unit {\n");

	AppendCString(&string, "  init: %d (bool)\n", s->init);

	AppendCString(&string, "  name: ");
	AppendString(&string, MetaString(&s->name));
	AppendCString(&string, "(String)\n");

	AppendCString(&string, "  team: ");
	AppendString(&string, MetaString(&s->team));
	AppendCString(&string, "(Team)\n");

	AppendCString(&string, "  cur_traits: ");
	AppendString(&string, MetaString(&s->cur_traits));
	AppendCString(&string, "(TraitSet)\n");

	AppendCString(&string, "  max_traits: ");
	AppendString(&string, MetaString(&s->max_traits));
	AppendCString(&string, "(TraitSet)\n");

	AppendCString(&string, "  cur_action_points: %d (int)\n", s->cur_action_points);

	AppendCString(&string, "  max_action_points: %d (int)\n", s->max_action_points);

	AppendCString(&string, "  ability_ids: %p (Id<Ability>[])\n", s->ability_ids);

	AppendCString(&string, "  intent: ");
	AppendString(&string, MetaString(&s->intent));
	AppendCString(&string, "(Intent)\n");

	AppendCString(&string, "  slot_pos: ");
	AppendString(&string, MetaString(&s->slot_pos));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// utf32string.h
// ------------------------------------------

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

// ---------------FILE START---------------
// util.h
// ------------------------------------------

// ---------------FILE START---------------
// vec.h
// ------------------------------------------

String MetaString(const Vec2i *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Vec2i {\n");

	AppendCString(&string, "  x: %d (int)\n", s->x);

	AppendCString(&string, "  y: %d (int)\n", s->y);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const Vec2f *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Vec2f {\n");

	AppendCString(&string, "  x: %f (float)\n", s->x);

	AppendCString(&string, "  y: %f (float)\n", s->y);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const Vec3f *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Vec3f {\n");

	AppendCString(&string, "  x: %f (float)\n", s->x);

	AppendCString(&string, "  y: %f (float)\n", s->y);

	AppendCString(&string, "  z: %f (float)\n", s->z);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const Vec4f *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Vec4f {\n");

	AppendCString(&string, "  x: %f (float)\n", s->x);

	AppendCString(&string, "  y: %f (float)\n", s->y);

	AppendCString(&string, "  z: %f (float)\n", s->z);

	AppendCString(&string, "  w: %f (float)\n", s->w);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const Mat3f *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Mat3f {\n");

	AppendCString(&string, "  xx: %f (float)\n", s->xx);

	AppendCString(&string, "  xy: %f (float)\n", s->xy);

	AppendCString(&string, "  xz: %f (float)\n", s->xz);

	AppendCString(&string, "  yx: %f (float)\n", s->yx);

	AppendCString(&string, "  yy: %f (float)\n", s->yy);

	AppendCString(&string, "  yz: %f (float)\n", s->yz);

	AppendCString(&string, "  zx: %f (float)\n", s->zx);

	AppendCString(&string, "  zy: %f (float)\n", s->zy);

	AppendCString(&string, "  zz: %f (float)\n", s->zz);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const Mat4f *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Mat4f {\n");

	AppendCString(&string, "  xx: %f (float)\n", s->xx);

	AppendCString(&string, "  xy: %f (float)\n", s->xy);

	AppendCString(&string, "  xz: %f (float)\n", s->xz);

	AppendCString(&string, "  xw: %f (float)\n", s->xw);

	AppendCString(&string, "  yx: %f (float)\n", s->yx);

	AppendCString(&string, "  yy: %f (float)\n", s->yy);

	AppendCString(&string, "  yz: %f (float)\n", s->yz);

	AppendCString(&string, "  yw: %f (float)\n", s->yw);

	AppendCString(&string, "  zx: %f (float)\n", s->zx);

	AppendCString(&string, "  zy: %f (float)\n", s->zy);

	AppendCString(&string, "  zz: %f (float)\n", s->zz);

	AppendCString(&string, "  zw: %f (float)\n", s->zw);

	AppendCString(&string, "  wx: %f (float)\n", s->wx);

	AppendCString(&string, "  wy: %f (float)\n", s->wy);

	AppendCString(&string, "  wz: %f (float)\n", s->wz);

	AppendCString(&string, "  ww: %f (float)\n", s->ww);

	AppendCString(&string, "}");

	return string;
}

String MetaString(const Rect *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Rect {\n");

	AppendCString(&string, "  pos: ");
	AppendString(&string, MetaString(&s->pos));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "  size: ");
	AppendString(&string, MetaString(&s->size));
	AppendCString(&string, "(Vec2f)\n");

	AppendCString(&string, "}");

	return string;
}

// ---------------FILE START---------------
// work_entry.h
// ------------------------------------------
