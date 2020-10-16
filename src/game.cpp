#include <intrin.h>
#include <ctime>

#include "macros.h"
#include "const.h"
#include "global.h"
#include "debug.h"

#include "platform.h"
#include "opengl.h"
#include "game.h"

#define MAX_SHADER_SOURCE_SIZE 10000
#define MAX_SHADER_LOG_SIZE 10000

Platform *platform = nullptr;
OpenGL *gl = nullptr;
Game *game = nullptr;

#include "meta_print.cpp"

#include "ability.cpp"
#include "game_code_include.cpp"

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
    TIMED_BLOCK;

    memory::per_frame_arena = AllocArena();
    memory::permanent_arena = AllocArena();

    #if 0
    InitLcgSystemSeed(&random::default_lcg);
    #else
    InitLcgSetSeed(&random::default_lcg, 42685076);
    #endif

    //TestDistributionAndLog();

    game->temp_texture = GenerateAndBindTexture();
    gl->Enable(GL_BLEND);
    gl->BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    FT_Library ft_lib;
    if(InitFreetype(&ft_lib))
    {
        FT_Face roboto_face = LoadFontFaceFromFile("resource/Roboto-Bold.ttf", ft_lib);
        text_render::default_font = LoadFontData(roboto_face, 256);
        CloseFreetype(&ft_lib);
    }

    input::global_input = &game->input;

    game->color_shader = GenerateShaderProgramFromFiles("src/color_vertex.glsl", "src/color_fragment.glsl");
    gl->BindFragDataLocation(game->color_shader, 0, "frag_color");
    gl->ProgramUniform4f(game->color_shader, 1, 1.f, 1.f, 1.f, 1.f); // draw color; default white

    game->uv_shader = GenerateShaderProgramFromFiles("src/uv_vertex.glsl", "src/uv_fragment.glsl");
    gl->BindFragDataLocation(game->uv_shader, 0, "frag_color");
    gl->ProgramUniform1i(game->uv_shader, 1, 0); // sampling texture index; default 0 index

    gl->ProgramUniform2fv(game->color_shader, 0, 1, (GLfloat*)&game->window_size);
    gl->ProgramUniform2fv(game->uv_shader, 0, 1, (GLfloat*)&game->window_size);

    //SetCameraPos(0.5f*game->window_size);
    SetCameraPos(&game->camera, {0.f,0.f});
    // game->camera_pos = 0.5f*game->window_size; //{0.f,0.f};
    // gl->ProgramUniform2fv(game->color_shader, 4, 1, (GLfloat*)&game->camera_pos);
    // gl->ProgramUniform2fv(game->uv_shader, 4, 1, (GLfloat*)&game->camera_pos);

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
    gl->Enable(GL_BLEND);


    // Ability table
    g::ability_table = AllocTable<Ability>(100);
    LoadAbilityFile("data/ability.dat", &g::ability_table);
    auto ability_table_copy = &g::ability_table;

    // Unit breed table
    g::breed_table = AllocTable<Breed>(100);
    LoadBreedFile("data/breed.dat", &g::breed_table, g::ability_table);
    auto breed_table_copy = &g::breed_table;

    // Unit table
    g::unit_table = AllocTable<Unit>(100);
    auto unit_table_copy = &g::unit_table;

    // Passive skill table
    // g::passive_skill_table = AllocTable(sizeof(PassiveSkill), c::passive_skill_table_partition_size);
    // {
    //  PassiveSkill passives[] = {{"Equilibrium"}, {"Potency"}, {"Constitution"}, {"Bravery"}, {"Stoicism"}};

    //  for(PassiveSkill passive : passives)
    //  {
    //      *(PassiveSkill*)CreateEntry(&g::passive_skill_table) = passive;
    //  }
    // }

    AddUnitToUnitSet(CreateUnitByName(StringFromCString("Warrior"), Team::allies), &game->player_party);
    AddUnitToUnitSet(CreateUnitByName(StringFromCString("Rogue"), Team::allies), &game->player_party);
    AddUnitToUnitSet(CreateUnitByName(StringFromCString("Archer"), Team::allies), &game->player_party);
    AddUnitToUnitSet(CreateUnitByName(StringFromCString("Cleric"), Team::allies), &game->player_party);

    game->current_battle = {};
    game->current_battle.hud = {{0.f, game->window_size.y-c::hud_offset_from_bottom}, {game->window_size.x, c::hud_offset_from_bottom}};

    // CreateUnit("Wolf", Team::enemies, &game->enemies[0]);
    // CreateUnit("Slime", Team::enemies, &game->enemies[1]);

    // Generate unit slots
    {
        Vec2f pos = {50.f, 300.f};

        for(Vec2f &slot : game->current_battle.unit_slots)
        {
            slot = pos;
            pos.x += c::unit_slot_size.x + c::unit_slot_padding;
        }
    }

    // Place units in unit slots
    // for(int i=0; i<c::max_party_size; i++)
    // {
    //  game->current_battle.units[i] = game->player_party[i];
    // }
    //game->current_battle.units[c::max_party_size+0] = CreateUnitByName("Dragon", Team::enemies);
    // game->current_battle.units[c::max_party_size+1] = CreateUnitByName("Slime", Team::enemies);
    // game->current_battle.units[c::max_party_size+2] = CreateUnitByName("Wolf", Team::enemies);
    // game->current_battle.units[c::max_party_size+3] = CreateUnitByName("Slime", Team::enemies);

    // imgui
    game->debug_container = c::def_ui_container;
    game->debug_container.max_size = {300.f,200.f};

    // Cursor
    game->pointer_cursor = LoadBitmapFileIntoSprite("resource/cursor.bmp");
    game->target_cursor = LoadBitmapFileIntoSprite("resource/target.bmp", c::align_center);
    game->red_target_cursor = LoadBitmapFileIntoSprite("resource/target_red.bmp", c::align_center);

    UnitSet battle_units = game->player_party;
    // for(int i=0; i<4; ++i)
    // {
    //     int random_index = RandomU32(6, g::breed_table.entry_count-1);
    //     Id breed_id = g::breed_table.entries[random_index].id;
    //     AddUnitToUnitSet(CreateUnit(breed_id, Team::enemies), &battle_units);
    // }

    AddUnitToUnitSet(CreateUnitByName(StringFromCString("Dragon"), Team::enemies), &battle_units);
    //AddUnitToUnitSet(CreateUnitByName(StringFromCString("Slime"), Team::enemies), &battle_units);
    //AddUnitToUnitSet(CreateUnitByName(StringFromCString("Wolf"), Team::enemies), &battle_units);
    //AddUnitToUnitSet(CreateUnitByName(StringFromCString("Mage"), Team::enemies), &battle_units);
    //AddUnitToUnitSet(CreateUnitByName(StringFromCString("Slime"), Team::enemies), &battle_units);
    //AddUnitToUnitSet(CreateUnitByName(StringFromCString("Wolf"), Team::enemies), &battle_units);
    //AddUnitToUnitSet(CreateUnitByName(StringFromCString("Dragon"), Team::enemies), &battle_units);
    InitMainMenu(&game->mainmenu_state);
    InitBattle(&game->current_battle);
    InitCampaign(&game->campaign);

    StartBattle(&game->current_battle, battle_units);
    StartEditor(&game->editor_state);

    LoadKeybindsFromFile("data/default_keybinds.dat");

    // Debug
    debug::start_count = __rdtsc();
    //debug::timed_block_array_size = __COUNTER__;
    debug::cycles_per_second = platform->PerformanceCounterFrequency();
    game->draw_debug_text = false;
    game->test_float = 0.f;

    game->current_state = GameState::Test;

    // Test TraitSetLinearSpace()
    // for(int i=0; i<100; ++i)
    // {
    //     TraitSet start   = {(s32)RandomU32(0,5), (s32)RandomU32(0,5), (s32)RandomU32(0,5)};
    //     TraitSet current = {(s32)RandomU32(0,10), (s32)RandomU32(0,10), (s32)RandomU32(0,10)};
    //     TraitSet end     = {start.vigor + (s32)RandomU32(0,5), start.focus + (s32)RandomU32(0,5), start.armor + (s32)RandomU32(0,5)};
    //     float result = TraitSetLinearSpace(current, start, end);
    //     Log("(%d,%d,%d)V ... (%d,%d,%d)F ... (%d,%d,%d)A = %.2f",
    //         start.vigor, current.vigor, end.vigor,
    //         start.focus, current.focus, end.focus,
    //         start.armor, current.armor, end.armor,
    //         result);
    // }

    // DEPTH 1
    // int value_count = 3;
    // int permutation_count = m::Factorial(value_count);
    // Array<u8> values = CreateTempArray<u8>(value_count);
    // for(u8 i=0; i<value_count; ++i) values += i;

    // int dst_byte_count = value_count * permutation_count;
    // u8 *dst = (u8 *)malloc(dst_byte_count);
    // u8 *p = dst;
    // GenerateU8Permutations(values, dst, dst_byte_count);
    // for(int i=0; i<permutation_count; ++i)
    // {
    //     Log("%u %u %u", p[0], p[1], p[2]);
    //     p += 3;
    // }
    // free(dst);

    // To use this, set OPTIMIZE to true, convert the original function into a function pointer
    // with the name OPTIMIZING_FUNCTION, and declare two functions named with "Slow" and "Fast"
    // appended to the name set in OPTIMIZING_FUNCTION. aowtc: F12 toggles between slow and fast in
    // GameUpdateAndRender.
    // e.g., Foo, FooSlow, FooFast
    #define OPTIMIZE false
    #if OPTIMIZE
    #define OPTIMIZING_FUNCTION ClearArena
    #define SLOW_FUNCTION PASTE(OPTIMIZING_FUNCTION, Slow)
    #define FAST_FUNCTION PASTE(OPTIMIZING_FUNCTION, Fast)
    OPTIMIZING_FUNCTION = PASTE(OPTIMIZING_FUNCTION, Slow);
    #endif
}

extern "C" void
GameUpdateAndRender()
{
    //TIMED_BLOCK;
    debug::timed_block_array_size = __COUNTER__;
    ClearArena(&memory::per_frame_arena);

    //DrawText(c::def_text_layout, {}, "%d", game->

    // SetDrawDepth(1.f);
    // DrawTextMultiline(c::small_text_layout, MousePos(), MetaString(&game->pointer_cursor));

    // Utf32String foo = {};
    // foo.length = 5;
    // foo.max_length = 100;
    // foo.data = (u32*)0x123456;
    // String foo_as_string = AsString(foo);
    // //EasyDrawText("%.*s", (int)foo_as_string.length, foo_as_string.data);
    // DrawTextMultiline(c::def_text_layout, {0.f,0.f}, "%.*s", (int)foo_as_string.length, foo_as_string.data);

    if(g::error_flash_increasing)
    {
        g::error_flash_counter += c::error_flash_speed;
        if(g::error_flash_counter > 1.f) g::error_flash_increasing = false;
    }
    else
    {
        g::error_flash_counter -= c::error_flash_speed;
        if(g::error_flash_counter < 0.f) g::error_flash_increasing = true;
    }

    if(Pressed(vk::F12))
    {
        #if OPTIMIZE
        if(OPTIMIZING_FUNCTION == SLOW_FUNCTION) OPTIMIZING_FUNCTION = FAST_FUNCTION;
        else OPTIMIZING_FUNCTION = SLOW_FUNCTION;
        #endif
    }

    // Reset timed block data
    if(Pressed(vk::R))
    {
        for(int i=0; i<debug::timed_block_array_size; i++)
        {
            TIMED_BLOCK_ARRAY[i].hit_count = 0;
            TIMED_BLOCK_ARRAY[i].total_cycle_count = 0;
        }
    }

    // Toggle drawing timed block data
    if(Pressed(vk::tilde)) game->draw_debug_text = !game->draw_debug_text;

    Vec2f pos = {1600.f,0.f};
    TextLayout frametime_layout = c::def_text_layout;
    frametime_layout.align = c::align_topright;
    pos.y += DrawUiText(frametime_layout, pos, "frame: %.3fms", game->frame_time_ms).size.y;

    // Right click cancels selected ability if one is selected.
    // If no ability is selected, cancels selected unit if one is selected.
    // If neither a unit nor ability is selected, do nothing
    // @todo: this should be transferred to the UpdateBattle() method probably.
//  if(Pressed(vk::rmb))
//  {
//      if(game->current_battle.selected_ability != nullptr)
//      {
//          game->current_battle.selected_ability = nullptr;
// //           game->current_battle.selected_ability_valid_target_set = {};
//      }
//      else if(game->current_battle.selected_unit != nullptr)
//      {
//          game->current_battle.selected_unit = nullptr;
//      }
//  }

    // Reset pen position to container origin for all gui containers
    ResetImguiContainer(&game->debug_container);

    // Function keys to switch between game states.
    if(Pressed(vk::F1)) game->current_state = GameState::Battle;
    if(Pressed(vk::F2)) game->current_state = GameState::Editor;

    GameState new_state = GameState::None;
    if(game->current_state == GameState::MainMenu)
    {
        new_state = TickMainMenu(&game->mainmenu_state, game->state_entered);
    }
    else if(game->current_state == GameState::Battle)
    {
        new_state = TickBattle(&game->current_battle);
    }
    else if(game->current_state == GameState::Editor)
    {
        new_state = TickEditor(&game->editor_state);
    }
    else if(game->current_state == GameState::Options)
    {
        new_state = TickOptionsMenu(&game->options_menu);
    }
    else if(game->current_state == GameState::Campaign)
    {
        new_state = TickCampaign(&game->campaign);
    }
    else if(game->current_state == GameState::AiExplorer)
    {
        new_state = TickAiExplorer(&game->ai_explorer);
    }
    else if(game->current_state == GameState::Test)
    {
        new_state = TickTestMode(&game->test_mode);
    }

    game->state_entered = false;
    if(new_state != GameState::None)
    {
        game->state_entered = true;
        game->current_state = new_state;
        if(game->current_state == GameState::Quit) game->exit_requested = true;
    }

    if(game->draw_debug_text)
    {
        DrawTimedBlockData();
    }

    //DrawTextMultiline(c::def_text_layout, {}, "#: %d", game->number_of_arenas_allocated);

    // Log timed block data and exit game.
    #if 1
    if(game->exit_requested)
    {
       LogToFile("logs/DebugTimings.txt", "-----------------------");
        for(int i=0; i<debug::timed_block_array_size; i++)
        {
            TimedBlockEntry *entry = TIMED_BLOCK_ARRAY+i;
            if(entry->function_name == nullptr) continue;

            if(entry->hit_count > 0)
            {
                LogToFile("logs/DebugTimings.txt",
                          "%s : (hit %llu, cycles %llu, c/h %llu)",
                          entry->function_name,
                          entry->hit_count,
                          entry->total_cycle_count,
                          entry->total_cycle_count/entry->hit_count);
            }
            else
            {
                LogToFile("logs/DebugTimings.txt",
                          "%s : (hit %llu, cycles %llu, c/h ?)",
                          entry->function_name,
                          entry->hit_count,
                          entry->total_cycle_count);
            }

        }
    }
    #endif
}

TimedBlockEntry TIMED_BLOCK_ARRAY[__COUNTER__-1];