#include "ability.h"
#include "ability_button.h"
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
String MetaString(const AbilityTier *s);

String MetaString(const Ability *s);

String MetaString(const AiExplorer *s);

String MetaString(const AlignX *s);

String MetaString(const AlignY *s);

String MetaString(const Align *s);

template<typename Type>
String MetaString(const Array<Type> *s);

String MetaString(const BattleEvent *s);

String MetaString(const BattlePhase *s);

String MetaString(const BattleState *s);

String MetaString(const Battle *s);

String MetaString(const BgraPixel *s);

String MetaString(const Bitmap *s);

String MetaString(const Buffer *s);

String MetaString(const Camera *s);

String MetaString(const CampaignState *s);

String MetaString(const Campaign *s);

String MetaString(const Color *s);

String MetaString(const TimedBlockEntry *s);

String MetaString(const TimedBlock *s);

String MetaString(const OverlayOption *s);

String MetaString(const DebugOverlay *s);

String MetaString(const Texture *s);

String MetaString(const Framebuffer *s);

String MetaString(const DirectedLineLayout *s);

String MetaString(const EditorMode *s);

String MetaString(const InputElementType *s);

String MetaString(const AbilityPropertyIndex *s);

String MetaString(const BreedPropertyIndex *s);

String MetaString(const InputElement *s);

String MetaString(const Editor *s);

String MetaString(const EffectType *s);

String MetaString(const Effect *s);

String MetaString(const EffectParams_Damage *s);

String MetaString(const EffectParams_DamageIgnoreArmor *s);

String MetaString(const EffectParams_Restore *s);

String MetaString(const EffectParams_Gift *s);

String MetaString(const EffectParams_Steal *s);

String MetaString(const BattleScore *s);

String MetaString(const AiIntent *s);

template<typename Type>
String MetaString(const FixedArray<Type> *s);

String MetaString(const FrametimeGraphState *s);

String MetaString(const GameInitData *s);

String MetaString(const GameState *s);

String MetaString(const GenerateNodeGraph_Params *s);

String MetaString(const ListPanelLayout *s);

String MetaString(const ListPanelResponse *s);

String MetaString(const ListPanel_ *s);

String MetaString(const ButtonLayout *s);

String MetaString(const TextEntryLayout *s);

String MetaString(const IntegerBoxLayout *s);

String MetaString(const IntegerBoxResponse *s);

String MetaString(const ImguiContainer *s);

String MetaString(const ButtonResponse *s);

String MetaString(const TextEntryResponse *s);

String MetaString(const InputState *s);

String MetaString(const VirtualKey *s);

String MetaString(const Intent *s);

String MetaString(const KeyBind *s);

String MetaString(const LogState *s);

String MetaString(const MainMenu *s);

String MetaString(const MapResponse *s);

String MetaString(const Arena *s);

String MetaString(const Node *s);

String MetaString(const Edge *s);

String MetaString(const NodeGraph *s);

String MetaString(const NodeGraphResponse *s);

String MetaString(const ForceSimParams *s);

String MetaString(const ForceSimInvalidationReason *s);

String MetaString(const ForceSimState *s);

String MetaString(const OptionsMenu *s);

String MetaString(const OscillatingTimer *s);

String MetaString(const PassiveSkill *s);

String MetaString(const PassiveNode *s);

String MetaString(const PassiveSkillTree *s);

template<typename Type>
String MetaString(const PoolId<Type> *s);

template<typename Type>
String MetaString(const PoolEntry<Type> *s);

template<typename Type>
String MetaString(const Pool<Type> *s);

String MetaString(const LCG *s);

String MetaString(const RingBuffer *s);

String MetaString(const RoomType *s);

String MetaString(const Room *s);

String MetaString(const Sprite *s);

String MetaString(const String *s);

template<typename Type>
String MetaString(const Id<Type> *s);

template<typename Type>
String MetaString(const TableEntry<Type> *s);

template<typename Type>
String MetaString(const Table<Type> *s);

String MetaString(const TableDrawMode *s);

String MetaString(const TableDrawState *s);

String MetaString(const TargetClass *s);

String MetaString(const TestMode *s);

String MetaString(const TokenType_ *s);

String MetaString(const Token *s);

String MetaString(const Font *s);

String MetaString(const TextLayout *s);

String MetaString(const TextResponse *s);

String MetaString(const Timer *s);

String MetaString(const TraitSet *s);

String MetaString(const Team *s);

String MetaString(const Breed *s);

String MetaString(const Unit *s);

String MetaString(const Utf32String *s);

String MetaString(const Vec2i *s);

String MetaString(const Vec2f *s);

String MetaString(const Vec3f *s);

String MetaString(const Vec4f *s);

String MetaString(const Mat3f *s);

String MetaString(const Mat4f *s);

String MetaString(const Rect *s);

