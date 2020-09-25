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
	"Self",
	"Single Ally",
	"Single Ally (Not Self)",
	"All Allies",
	"All Allies (Not Self)",
	"Single Enemy",
	"All Enemies",
	"Single Unit",
	"Single Unit (Not Self)",
	"All"
};

#endif