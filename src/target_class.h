#ifndef TARGET_CLASS_H
#define TARGET_CLASS_H

enum class TargetClass
{
	self,
	single_ally,
	single_ally_not_self,
	all_allies,
	all_allies_not_self,
	single_enemy,
	all_enemies,
	single_unit,
	single_unit_not_self,
	all_units
};

const char *TargetClass_filestrings[] = {
	"self",
	"single_ally",
	"single_ally_not_self",
	"all_allies",
	"all_allies_not_self",
	"single_enemy",
	"all_enemies",
	"single_unit",
	"single_unit_not_self",
	"all_units"
};

const char *TargetClass_userstrings[] = {
	"self",
	"single ally",
	"single ally (not self)",
	"all allies",
	"all allies (not self)",
	"single enemy",
	"all enemies",
	"single unit",
	"single unit (not self)",
	"all"
};

bool ParseNextAsTargetClass(Buffer *buffer, TargetClass *target_class);

#endif