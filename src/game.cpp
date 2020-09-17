#include "ft2build.h"
#include FT_FREETYPE_H

#include "platform.h"
#include "opengl.h"
#include "game.h"

#include "const.h"

#define MAX_SHADER_SOURCE_SIZE 10000
#define MAX_SHADER_LOG_SIZE 10000

Platform *platform = nullptr;
OpenGL *gl = nullptr;
Game *game = nullptr;

#include "log.cpp"
#include "util.cpp"
#include "opengl.cpp"
#include "math.cpp"
#include "bitmap.cpp"
#include "vec.cpp"
#include "strings.cpp"
//#include "csv.cpp"
#include "text_render.cpp"
#include "freetype_wrapper.cpp"
#include "text_parsing.cpp"
#include "data_table.cpp"
#include "imgui.cpp"
#include "draw.cpp"
#include "input.cpp"
#include "image.cpp"
#include "types.cpp"
#include "sprite.cpp"

#include "unit.cpp"

extern "C" void
GameHook(Platform *platform_, OpenGL *gl_, Game *game_)
{
	platform = platform_;
	gl = gl_;
	game = game_;
}

extern "C" void
GameInit()
{
	input::global_input = &game->input;

	game->color_shader = GenerateShaderProgramFromFiles("src/color_vertex.glsl", "src/color_fragment.glsl");
	gl->BindFragDataLocation(game->color_shader, 0, "frag_color");
	gl->ProgramUniform4f(game->color_shader, 1, 1.f, 1.f, 1.f, 1.f); // draw color; default white

	game->uv_shader = GenerateShaderProgramFromFiles("src/uv_vertex.glsl", "src/uv_fragment.glsl");
	gl->BindFragDataLocation(game->uv_shader, 0, "frag_color");
	gl->ProgramUniform1i(game->uv_shader, 1, 0); // sampling texture index; default 0 index

	game->window_size = {800.f, 600.f};
	gl->ProgramUniform2fv(game->color_shader, 0, 1, (GLfloat*)&game->window_size);
	gl->ProgramUniform2fv(game->uv_shader, 0, 1, (GLfloat*)&game->window_size);

	// Set up UV VAO/VBO
	gl->GenVertexArrays(1, &game->uv_vao);
	gl->BindVertexArray(game->uv_vao);
	gl->GenBuffers(1, &game->uv_vbo);
	gl->BindBuffer(GL_ARRAY_BUFFER, game->uv_vbo);
	gl->EnableVertexAttribArray(0);
	gl->EnableVertexAttribArray(1);
	gl->VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
							4*sizeof(GLfloat), 0);
	gl->VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
	 						4*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));

	// Set up color VAO/VBO
	gl->GenVertexArrays(1, &game->color_vao);
	gl->BindVertexArray(game->color_vao);
	gl->GenBuffers(1, &game->color_vbo);
	gl->BindBuffer(GL_ARRAY_BUFFER, game->color_vbo);
	gl->EnableVertexAttribArray(0);
	gl->VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
							0, 0);

	gl->PixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//gl->Enable(GL_BLEND);

	if(InitFreetype(&game->ft_lib))
	{
		game->default_font = LoadFontFromFile("resource/Roboto-Bold.ttf", game->ft_lib);
	}

	game->temp_texture = GenerateAndBindTexture();

	// Game
	game->ability_table = AllocDataTable(sizeof(Ability), c::ability_table_partition_size);
	LoadAbilityFile("data/ability.dat", &game->ability_table);

	game->unit_table = AllocDataTable(sizeof(UnitSchematic), c::unit_table_partition_size);
	LoadUnitFile("data/unit.dat", &game->unit_table, game->ability_table);

	CreateUnit("Rogue", Team::allies, &game->allies[0]);
	CreateUnit("Warrior", Team::allies, &game->allies[1]);
	CreateUnit("Archer", Team::allies, &game->allies[2]);

	CreateUnit("Wolf", Team::enemies, &game->enemies[0]);
	CreateUnit("Slime", Team::enemies, &game->enemies[1]);

	for(Unit &unit : game->allies)
	{
		// void AddUnitToTargetSet(Unit *unit, TargetSet *target_set)
		AddUnitToTargetSet(&unit, &game->all_targets);
	}
	for(Unit &unit : game->enemies)
	{
		AddUnitToTargetSet(&unit, &game->all_targets);
	}

	// imgui
	game->debug_container.pos = {0.f,0.f};
	game->debug_container.max_size = {300.f,200.f};
	game->debug_container.pen = {0.f,0.f};
	game->debug_container.font_size = 24;

	game->ally_container.pos = {100.f, 200.f};
	game->ally_container.max_size = {0.f, 0.f};
	game->ally_container.pen = {0.f,0.f};
	game->ally_container.font_size = 24;

	game->enemy_container = game->ally_container;
	game->enemy_container.pos = {400.f, 200.f};

	// Cursor
	//platform->HideCursor();
	game->pointer_cursor = LoadBitmapFileIntoSprite("resource/cursor.bmp");
	game->target_cursor = LoadBitmapFileIntoSprite("resource/target.bmp", Alignment::center);
}

extern "C" void
GameUpdateAndRender()
{
	// Right click cancels all selections.
	if(Pressed(vk::rmb))
	{
		game->selected_unit = nullptr;
		SetSelectedAbility(nullptr);
	}

	// Reset pen position to container origin for all gui containers
	ResetImguiContainer(&game->debug_container);
	ResetImguiContainer(&game->ally_container);
	ResetImguiContainer(&game->enemy_container);

	// Clear inferred target set.
	// We build it from scratch every frame.
	game->_inferred_target_set = {};


	// Process ally unit buttons
	SetActiveContainer(&game->ally_container);
	for(Unit &unit : game->allies)
	{
		// Skip inactive units
		if(!unit.active) continue;

		// Get unit name for button label
		const char *unit_name = GetUnitName(unit);
		if(unit_name == nullptr) continue;

		unit.last_button_pos = game->ally_container.pos + game->ally_container.pen;

		if(game->_selected_ability != nullptr)
		{
			// An ability is selected
			auto response = ButtonColor(c::grey, c::red, unit_name);
			if(response.pressed)
			{
				// Unit button was clicked while an ability was selected/targeting
				//if(CheckValidAbilityTarget(game->selected_unit, &unit, ))
				{
					// The target selected for the ability is a valid target
					//ApplyAbilityToTarget(source, target, ability);
					game->test_int++;
				}
			}
			if(response.hovered)
			{
				game->_inferred_target_set =
					GenerateInferredTargetSet(game->selected_unit, &unit,
											  game->_selected_ability, game->all_targets);
			}
		}
		else
		{
			// No ability is selected
			auto response = ButtonColor(c::grey, c::white, unit_name);
			if(response.pressed)
			{
				game->selected_unit = &unit;
				SetSelectedAbility(nullptr);
			}
		}
	}

	// Process enemy unit buttons
	SetActiveContainer(&game->enemy_container);
	for(Unit &unit : game->enemies)
	{
		// Skip inactive units
		if(!unit.active) continue;

		// Get unit name for button label
		const char *unit_name = GetUnitName(unit);
		if(unit_name == nullptr) continue;

		Color button_color, hover_color;
		if(game->_selected_ability != nullptr)
		{
			auto response = ButtonColor(c::grey, c::red, unit_name);
			if(response.pressed)
			{
				// Unit button was clicked while an ability was selected/targeting
				//if(CheckValidAbilityTarget(game->selected_unit, &unit, game->selected_ability))
				{
					// The target selected for the ability is a valid target
					//ApplyAbilityToTarget(source, target, ability);
					game->test_int++;
				}
			}
			if(response.hovered)
			{
				game->_inferred_target_set =
					GenerateInferredTargetSet(game->selected_unit, &unit,
											  game->_selected_ability, game->all_targets);
			}
		}
		else
		{
			auto response = ButtonColor(c::grey, c::white, unit_name);
			if(response.pressed)
			{
				game->selected_unit = &unit;
				SetSelectedAbility(nullptr);
			}
		}
	}

	for(Unit *unit : game->_inferred_target_set.units)
	{
		float active_box_size = game->target_cursor.size.x;
		float offset = -0.5f*GetButtonHeight();
		Vec2f pos = imgui::active_container->pos + imgui::active_container->pen + Vec2f{offset,offset};
		pos = {pos.x, pos.y};

		DrawSprite(game->target_cursor, Round(pos));
	}

	// Draw unit information on HUD for currently selected unit.
	if(game->selected_unit != nullptr)
	{
		DrawUnitHudData(*game->selected_unit);
	}

	// Draw cursor
	Sprite cursor;
	if(game->_selected_ability == nullptr) cursor = game->pointer_cursor;
	else cursor = game->target_cursor;
	DrawSprite(cursor, game->mouse_pos);

	// Test
	DrawText(16, {0.f,0.f}, "ability count: %d", game->test_int);
}