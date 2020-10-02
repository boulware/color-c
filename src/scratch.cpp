template <typename Type>
DrawEditableMetaData()
{
	for(int i=0; i<meta_data.member_count; i++)
	{
	}
}







String MetaString(const AbilityTier *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "AbilityTier {\n");
	AppendCString(&string, "  init: %d (bool)\n", s->init);

	AppendCString(&string, "  required_traits: ");
	AppendString(&string, MetaString(s->required_traits));
	AppendCString(&string, "(TraitSet)\n");

	AppendCString(&string, "  effects: [invalid metadata] (Effect)\n", s->effects);
	AppendCString(&string, "}");

	return string;
}