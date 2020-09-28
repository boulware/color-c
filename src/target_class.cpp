#include "target_class.h"

bool
ParseNextAsTargetClass(Buffer *buffer, TargetClass *target_class)
{
	char *initial = buffer->p;

	Token target_class_token = NextToken(buffer);

	bool found_match = false;
	for(int i=0; i<ArrayCount(TargetClass_filestrings); i++)
	{
		if(TokenMatchesString(target_class_token, TargetClass_filestrings[i]))
		{
			*target_class = (TargetClass)i;
			found_match = true;
			break;
		}
	}

	if(!found_match) buffer->p = initial;
	return(found_match);
}