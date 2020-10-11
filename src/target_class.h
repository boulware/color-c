#ifndef TARGET_CLASS_H
#define TARGET_CLASS_H

enum class TargetClass : int
{
	none,
	self,
	single_ally,
	single_ally_not_self,
	all_allies,
	all_allies_not_self,
	single_enemy,
	all_enemies,
	single_unit,
	single_unit_not_self,
	all_units,
	COUNT
};

const char *_TargetClass_filestrings[] = {
	"[none]"                ,
	"self"                  ,
	"single_ally"           ,
	"single_ally_not_self"  ,
	"all_allies"            ,
	"all_allies_not_self"   ,
	"single_enemy"          ,
	"all_enemies"           ,
	"single_unit"           ,
	"single_unit_not_self"  ,
	"all_units"
}; //static_assert(ArrayCount(TargetClass_filestrings) == (int)TargetClass::COUNT);

const char *_TargetClass_userstrings[] = {
	"[none]" 					,
	"self" 						,
	"single ally" 				,
	"single ally (not self)" 	,
	"all allies" 				,
	"all allies (not self)" 	,
	"single enemy" 				,
	"all enemies" 				,
	"single unit" 				,
	"single unit (not self)" 	,
	"all"
}; //static_assert(ArrayCount(TargetClass_userstrings) == TargetClass::COUNT);

const char *TargetClassToUserString(TargetClass tc);
const char *TargetClassToFileString(TargetClass tc);
bool ParseNextAsTargetClass(Buffer *buffer, TargetClass *target_class);

#endif