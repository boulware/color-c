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
#include "game_code_include.cpp"

extern "C" void
GameHook(Platform *platform_, OpenGL *gl_, Game *game_)
{
    platform = platform_;
    gl = gl_;
    game = game_;
}

extern "C" void
GameInit(GameInitData init_data)
{
    // During init, enable debug output
    gl->Enable              ( GL_DEBUG_OUTPUT );
    gl->DebugMessageCallback( GlDebugMessageCallback, 0 );

    LogGlVersion();

    memory::arena_pool_mutex_handle = init_data.arena_pool_mutex_handle;
    memory::per_frame_arena_id = init_data.per_frame_arena_id;
    memory::permanent_arena_id = init_data.permanent_arena_id;

    game->current_state = GameState::Campaign;

    InitTableDrawState(&game->table_draw_state);

    #if 1
        InitLcgSystemSeed(&random::default_lcg);
    #else
        InitLcgSetSeed(&random::default_lcg, 505309726);
    #endif

    //TestDistributionAndLog();

    game->temp_screen_texture = CreateEmptyTexture((int)game->window_size.x, (int)game->window_size.y);

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
    gl->ProgramUniform2fv(game->color_shader, 0, 1, (GLfloat*)&game->window_size);

    game->uv_shader = GenerateShaderProgramFromFiles("src/uv_vertex.glsl", "src/uv_fragment.glsl");
    gl->BindFragDataLocation(game->uv_shader, 0, "frag_color");
    gl->ProgramUniform1i(game->uv_shader, 1, 0); // sampling texture index; default 0 index
    gl->ProgramUniform2fv(game->uv_shader, 0, 1, (GLfloat*)&game->window_size);

    SetCameraPos(&game->camera, {0.f,0.f});

    game->blur_shader = GenerateComputeShaderFromFile("src/shader/gaussian_blur.cs.glsl");
    gl->ProgramUniform1f(game->blur_shader, 2, 2.f);

    game->outline_shader = GenerateComputeShaderFromFile("src/shader/outline.cs.glsl");


    //game->blur_texture = CreateEmptyTexture((int)game->window_size.x, (int)game->window_size.y);


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

    game->prepass_framebuffer = CreateFramebuffer((int)game->window_size.x, (int)game->window_size.y);

    gl->PixelStorei(GL_UNPACK_ALIGNMENT, 1);
    gl->Enable(GL_BLEND);

    { // Ability table
        auto arena_id = AllocArena("Ability Table");
        g::ability_table = AllocTable<Ability>(100, arena_id);
        LoadAbilityFile("data/ability.dat", &g::ability_table);
        g_ability_table = &g::ability_table;
    }

    { // Unit breed table
        auto arena_id = AllocArena("Breed Table");
        g::breed_table = AllocTable<Breed>(100, arena_id);
        LoadBreedFile("data/breed.dat", &g::breed_table, g::ability_table);
        g_breed_table = &g::breed_table;
    }

    { // Unit table
        auto arena_id = AllocArena("Unit Table");
        g::unit_table = AllocTable<Unit>(100, arena_id);
        g_unit_table = &g::unit_table;
    }

    { // Temp unit table
        auto arena_id = AllocArena("Temp Unit Table");
        g::temp_unit_table = AllocTable<Unit>(100, arena_id);
    }

    // Passive skill table
    // g::passive_skill_table = AllocTable(sizeof(PassiveSkill), c::passive_skill_table_partition_size);
    // {
    //  PassiveSkill passives[] = {{"Equilibrium"}, {"Potency"}, {"Constitution"}, {"Bravery"}, {"Stoicism"}};

    //  for(PassiveSkill passive : passives)
    //  {
    //      *(PassiveSkill*)CreateEntry(&g::passive_skill_table) = passive;
    //  }
    // }


    // imgui
    game->debug_container = c::def_ui_container;
    game->debug_container.button_layout.label_layout.font_size = 16.f;
    game->debug_container.max_size = {150.f,200.f};

    // Cursor
    game->pointer_cursor = LoadBitmapFileIntoSprite("resource/cursor.bmp");
    game->target_cursor = LoadBitmapFileIntoSprite("resource/target.bmp", c::align_center);
    game->red_target_cursor = LoadBitmapFileIntoSprite("resource/target_red.bmp", c::align_center);


    InitMainMenu(&game->mainmenu_state);
    InitCampaign(&game->campaign);

    StartEditor(&game->editor_state);

    LoadKeybindsFromFile("data/default_keybinds.dat");

    // Debug
    debug::start_count = __rdtsc();
    //debug::timed_block_array_size = __COUNTER__;
    debug::cycles_per_second = platform->PerformanceCounterFrequency();
    game->test_int = 0;
    game->test_float = 1.f;

    game->draw_debug_overlay = false;
    game->debug_overlay = {};
    // int work_grp_cnt[3];
    // gl->GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    // gl->GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    // gl->GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

    // Log("max global (total) work group counts x:%i y:%i z:%i\n",
    //     work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

    // int work_grp_size[3];
    // gl->GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    // gl->GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    // gl->GetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

    // Log("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
    //      work_grp_size[0], work_grp_size[1], work_grp_size[2]);

    // int work_grp_inv;
    // gl->GetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
    // Log("max local work group invocations %i\n", work_grp_inv);

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
    //Log("vvvvvvvvvvvvvvvvvvvvvvvvvvv\nFRAME START\n^^^^^^^^^^^^^^^^^^^^^^^^");

    //TIMED_BLOCK;
    debug::timed_block_array_size = __COUNTER__;
    ClearArena(memory::per_frame_arena_id);

    //gl->UseProgram(game->color_shader);
    gl->BindFramebuffer(GL_DRAW_FRAMEBUFFER, game->prepass_framebuffer.id);
    gl->NamedFramebufferTexture(game->prepass_framebuffer.id, GL_COLOR_ATTACHMENT0,
                                game->prepass_framebuffer.texture.id, 0);
    //gl->ClearColor(0.2f, 0.4f, 0.6f, 0.8f);
    gl->ClearColor(0.0f, 0.f, 0.f, 1.f);
    gl->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //opengl->UseProgram(game->prepass_shader);

    if(Pressed(vk::F11))
    { // Log average GPU frametime
        Log("Average GPU frame (last %d frames): %.3fms",
            game->gpu_frametime_graph_state->entry_count,
            GetAverageFrametime(game->gpu_frametime_graph_state));
    }

    // Reset pen position to container origin for all gui containers
    ResetImguiContainer(&game->debug_container);
    Camera pushed_camera = PushUiCamera();
    if(game->draw_debug_overlay)
    {
        SetDrawDepth(c::debug_control_draw_depth);
        SetActiveContainer(&game->debug_container);
        ButtonResponse response = {};

        for(int i=0; i<ArrayCount(game->debug_overlay.option_active); ++i)
        {
            if(game->debug_overlay.option_active[i])
            {
                game->debug_container.button_layout = c::active_debug_button;
            }
            else
            {
                game->debug_container.button_layout = c::inactive_debug_button;
                //game->debug_overlay.window_rects[i] = {};
            }

            auto response = Button(OverlayOption_userstrings[i]);
            if(response.pressed and TakeMouseFocus())
                game->debug_overlay.option_active[i] = !game->debug_overlay.option_active[i];
        }

        float cur_draw_depth = c::debug_window_draw_depth;
        int current_index = (int)OverlayOption::Arenas;
        if(game->debug_overlay.option_active[current_index])
        {
            Vec2f window_pos = game->debug_overlay.window_positions[current_index];
            //Rect arenas_window_rect = GetTimedBlockDataRect(window_pos);
            //game->debug_overlay.window_positions[(u8)OverlayOption::TimedBlocks] = timed_block_window_rect;
            SetDrawDepth(cur_draw_depth);
            cur_draw_depth += 0.1f;
            // DrawFilledRect(timed_block_window_rect, c::dk_grey, true);
            // DrawUnfilledRect(timed_block_window_rect, c::white, true);

            Rect window_rect = DrawArenas(game->arena_pool, window_pos);
            DrawText(c::debug_window_label_text, RectTopLeft(window_rect), OverlayOption_userstrings[(int)current_index]);

            if(MouseInRect(window_rect) and TakeMouseFocus())
            {
                if(Pressed(vk::LMB))
                    game->debug_overlay.dragging_index = current_index;
            }
        }

        current_index = (int)OverlayOption::TimedBlocks;
        if(game->debug_overlay.option_active[current_index])
        {
            Vec2f window_pos = game->debug_overlay.window_positions[current_index];
            Rect window_rect = GetTimedBlockDataRect(window_pos);
            //game->debug_overlay.window_positions[(u8)OverlayOption::TimedBlocks] = timed_block_window_rect;
            SetDrawDepth(cur_draw_depth);
            cur_draw_depth += 0.1f;
            DrawFilledRect(window_rect, c::vdk_red);
            DrawUnfilledRect(window_rect, c::white);
            DrawTimedBlockData(window_pos);
            DrawText(c::debug_window_label_text, RectTopLeft(window_rect), OverlayOption_userstrings[(int)current_index]);

            if(MouseInRect(window_rect) and TakeMouseFocus())
            {
                if(Pressed(vk::LMB))
                    game->debug_overlay.dragging_index = current_index;
            }
        }

        current_index = (int)OverlayOption::CpuFrametime;
        if(game->debug_overlay.option_active[current_index])
        {
            Vec2f window_pos = game->debug_overlay.window_positions[current_index];
            Vec2f window_size = {300.f,300.f};
            Rect window_rect = {window_pos, window_size};
            SetDrawDepth(cur_draw_depth);
            cur_draw_depth += 0.1f;
            DrawFilledRect(window_rect, c::vdk_red);
            DrawFrametimes(game->cpu_frametime_graph_state, window_rect);
            DrawUnfilledRect(window_rect, c::white);

            DrawText(c::debug_window_label_text, RectTopLeft(window_rect), OverlayOption_userstrings[(int)current_index]);

            if(MouseInRect(window_rect) and TakeMouseFocus())
            {
                if(Pressed(vk::LMB))
                    game->debug_overlay.dragging_index = current_index;
            }
        }

        current_index = (int)OverlayOption::GpuFrametime;
        if(game->debug_overlay.option_active[current_index])
        {
            Vec2f window_pos = game->debug_overlay.window_positions[current_index];
            Vec2f window_size = {300.f,300.f};
            Rect window_rect = {window_pos, window_size};
            SetDrawDepth(cur_draw_depth);
            cur_draw_depth += 0.1f;
            DrawFilledRect(window_rect, c::vdk_red);
            DrawFrametimes(game->gpu_frametime_graph_state, window_rect);
            DrawUnfilledRect(window_rect, c::white);
            DrawText(c::debug_window_label_text, RectTopLeft(window_rect), OverlayOption_userstrings[(int)current_index]);

            if(MouseInRect(window_rect) and TakeMouseFocus())
            {
                if(Pressed(vk::LMB))
                    game->debug_overlay.dragging_index = current_index;
            }
        }

        current_index = (int)OverlayOption::Tables;
        if(game->debug_overlay.option_active[current_index])
        {
            Vec2f window_pos = game->debug_overlay.window_positions[current_index];
            Vec2f window_size = {300.f,300.f};
            Rect window_rect = {window_pos, window_size};
            SetDrawDepth(cur_draw_depth);
            cur_draw_depth += 0.1f;
            DrawFilledRect(window_rect, c::vdk_red);

            if(game->table_draw_state.cur_mode == TableDrawMode::Ability) {
                DrawTable(&game->table_draw_state, &g::ability_table, window_rect);
            }
            else if(game->table_draw_state.cur_mode == TableDrawMode::Breed) {
                DrawTable(&game->table_draw_state, &g::breed_table, window_rect);
            }
            else if(game->table_draw_state.cur_mode == TableDrawMode::Unit) {
                DrawTable(&game->table_draw_state, &g::unit_table, window_rect);
            }
            else if(game->table_draw_state.cur_mode == TableDrawMode::TempUnit) {
                DrawTable(&game->table_draw_state, &g::temp_unit_table, window_rect);
            }

            DrawUnfilledRect(window_rect, c::white);
            DrawText(c::debug_window_label_text, RectTopLeft(window_rect), OverlayOption_userstrings[(int)current_index]);

            if(MouseInRect(window_rect) and TakeMouseFocus())
            {
                if(Pressed(vk::LMB))
                    game->debug_overlay.dragging_index = current_index;
            }
        }

        if(!Down(vk::LMB)) game->debug_overlay.dragging_index = -1;

        // Window dragging
        if(game->debug_overlay.dragging_index >= 0)
        {
            Vec2f d_mouse = MousePos() - PrevMousePos();
            game->debug_overlay.window_positions[game->debug_overlay.dragging_index] += d_mouse;
        }

        SetDrawDepth(c::debug_window_draw_depth);
        for(int i=0; i<ArrayCount(game->debug_overlay.window_positions); ++i)
        {
            //DrawFilledRect(game->debug_overlay.window_rects[i], c::gold, true);
        }
        //DrawTimedBlockData();
    }
    PopUiCamera(pushed_camera);

    // SetDrawDepth(5.f);
    // DrawTable(g::unit_table, {}, 200.f);


//    DrawDummyText(c::small_text_layout, {});

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
    if(Pressed(vk::tilde)) game->draw_debug_overlay = !game->draw_debug_overlay;

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


    // Function keys to switch between game states.
    if(Pressed(vk::F2)) game->current_state = GameState::Editor;

    GameState new_state = GameState::None;
    if(game->current_state == GameState::MainMenu)
    {
        new_state = TickMainMenu(&game->mainmenu_state, game->state_entered);
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

    //ApplyComputeShaderToScreenTexture(game->blur_shader, &game->prepass_framebuffer.texture);
    CopyFramebufferToScreen(game->prepass_framebuffer);

    // for(int i=0; i<game->arena_pool->entry_count; ++i)
    // {
    //     PoolEntry<Arena> *entry = (game->arena_pool->entries + i);

    //     Arena arena = entry->data;
    //     DrawArena(arena, {10.f,10.f + i*20.f});
    // }
}

TimedBlockEntry TIMED_BLOCK_ARRAY[__COUNTER__-1];