#include "keybinds.h"

#include "better_text_parsing.h"
#include "input.h"

void
LoadKeybindsFromFile(const char *filename)
{
	Buffer file;
	bool load_success = platform->LoadFileIntoSizedBufferAndNullTerminate(filename, &file);
	if(!load_success) return;

	Token token = {};

	bool invalid_syntax = false;
	while(!invalid_syntax and BufferBytesRemaining(file) > 0)
	{
		Token keybind_userstring_token = {};
		if(!NextTokenAsDoubleQuotedString(&file, &keybind_userstring_token)) invalid_syntax = true;
		if(!ConfirmNextTokenType(&file, TokenType_::Colon)) invalid_syntax = true;

		Token vk_userstring_token = NextToken(&file);
		if(vk_userstring_token.type != TokenType_::Identifier) invalid_syntax = true;

		int keybind_index = -1;
		for(int i=0; i<ArrayCount(keybinds::userstrings); ++i)
		{
			if(TokenMatchesStringIgnoreCase(keybind_userstring_token, keybinds::userstrings[i]))
			{
				keybind_index = i;
				break;
			}
		}

		if(keybind_index < 0)
		{
			Log(__FUNCSIG__"(): invalid syntax in file \"%s\"", filename);
		}

		for(u8 i=0; i<ArrayCount(VirtualKey_userstrings); ++i)
		{
			if(TokenMatchesStringIgnoreCase(vk_userstring_token, VirtualKey_userstrings[i]))
			{
				keybinds::vk_mappings[keybind_index] = i;
				break;
			}
		}
	}
}

u8 KeyBindToVirtualKey(KeyBind keybind)
{
	return keybinds::vk_mappings[(int)keybind];
}

bool Down(KeyBind keybind)
{
	u8 vk = KeyBindToVirtualKey(keybind);
	return Down(vk);
}

bool Pressed(KeyBind keybind)
{
	u8 vk = KeyBindToVirtualKey(keybind);
	return Pressed(vk);
}

bool Released(KeyBind keybind)
{
	u8 vk = KeyBindToVirtualKey(keybind);
	return Released(vk);
}

bool Repeated(KeyBind keybind)
{
	u8 vk = KeyBindToVirtualKey(keybind);
	return Repeated(vk);
}

bool PressedOrRepeated(KeyBind keybind)
{
	u8 vk = KeyBindToVirtualKey(keybind);
	return(Pressed(vk) or Repeated(vk));
}