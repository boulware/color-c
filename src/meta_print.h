#include "ability.h"
#include "align.h"
#include "array.h"
#include "battle.h"
#include "bitmap.h"
#include "color.h"
#include "const.h"
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
#include "opengl.h"
#include "oscillating_timer.h"
#include "passive_skill_tree.h"
#include "platform.h"
#include "random.h"
#include "sprite.h"
#include "string.h"
#include "table.h"
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

String MetaString(const AbilityTier *s);

String MetaString(const Ability *s);

String MetaString(const Align *s);

String MetaString(const Ability *s);

String MetaString(const Intent *s);

String MetaString(const BattleEvent *s);

String MetaString(const Battle *s);

String MetaString(const BgraPixel *s);

String MetaString(const Bitmap *s);

String MetaString(const Color *s);

String MetaString(const TimedBlockEntry *s);

String MetaString(const TimedBlock *s);

String MetaString(const InputElement *s);

String MetaString(const Editor *s);

String MetaString(const Effect *s);

String MetaString(const EffectParams_Damage *s);

String MetaString(const EffectParams_DamageIgnoreArmor *s);

String MetaString(const EffectParams_Restore *s);

String MetaString(const EffectParams_Gift *s);

String MetaString(const EffectParams_Steal *s);

String MetaString(const Game *s);

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

String MetaString(const LogState *s);

String MetaString(const Arena *s);

String MetaString(const Buffer *s);

String MetaString(const StringBuffer *s);

String MetaString(const Token *s);

String MetaString(const OpenGL *s);

String MetaString(const OscillatingTimer *s);

String MetaString(const PassiveSkill *s);

String MetaString(const PassiveNode *s);

String MetaString(const PassiveSkillTree *s);

String MetaString(const Platform *s);

String MetaString(const LCG *s);

String MetaString(const Sprite *s);

String MetaString(const String *s);

template<typename Type>
String MetaString(const TableEntry *s);

String MetaString(const Buffer *s);

String MetaString(const StringBuffer *s);

String MetaString(const Token *s);

String MetaString(const Font *s);

String MetaString(const TextLayout *s);

String MetaString(const Timer *s);

String MetaString(const TraitSet *s);

String MetaString(const Breed *s);

String MetaString(const Unit *s);

String MetaString(const UnitSlot *s);

String MetaString(const UnitSet *s);

String MetaString(const Utf32String *s);

String MetaString(const Vec2i *s);

String MetaString(const Vec2f *s);

String MetaString(const Vec3f *s);

String MetaString(const Vec4f *s);

String MetaString(const Mat3f *s);

String MetaString(const Mat4f *s);

String MetaString(const Rect *s);

