#include "target_class.h"

const char *TargetClassToUserString(TargetClass tc)
{
	return _TargetClass_userstrings[(int)tc];
}

const char *TargetClassToFileString(TargetClass tc)
{
	return _TargetClass_filestrings[(int)tc];
}

bool
ParseNextAsTargetClass(Buffer *buffer, TargetClass *target_class)
{
	char *initial = buffer->p;

	Token target_class_token = NextToken(buffer);

	bool found_match = false;
	for(int i=0; i<ArrayCount(_TargetClass_filestrings); i++)
	{
		if(TokenMatchesString(target_class_token, _TargetClass_filestrings[i]))
		{
			*target_class = (TargetClass)i;
			found_match = true;
			break;
		}
	}

	if(!found_match) buffer->p = initial;
	return(found_match);
}