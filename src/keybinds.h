#ifndef KEYBINDS_H
#define KEYBINDS_H

enum class KeyBind
{
	Select,
	Deselect,
	Exit,
	SelectUnit1,
	SelectUnit2,
	SelectUnit3,
	SelectUnit4,
	CycleUnits,
	SelectAbility1,
	SelectAbility2,
	SelectAbility3,
	SelectAbility4,
	COUNT
};

namespace keybinds
{
	u8 vk_mappings[KeyBind::COUNT] = {};
	const char *userstrings[KeyBind::COUNT] = {
		"Select",
		"Deselect",
		"Exit",
		"Select Unit 1",
		"Select Unit 2",
		"Select Unit 3",
		"Select Unit 4",
		"Cycle Units",
		"Select Ability 1",
		"Select Ability 2",
		"Select Ability 3",
		"Select Ability 4",
	};
};

u8 KeyBindToVirtualKey(KeyBind keybind);
bool Down(KeyBind keybind);
bool Pressed(KeyBind keybind);
bool Released(KeyBind keybind);
bool Repeated(KeyBind keybind);
bool PressedOrRepeated(KeyBind keybind);

#endif