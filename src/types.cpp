u32
ByteSwapU32(u32 in)
{
	u32 out;

	*(((u8*)&out) + 0) = *(((u8*)&in) + 3);
	*(((u8*)&out) + 1) = *(((u8*)&in) + 2);
	*(((u8*)&out) + 2) = *(((u8*)&in) + 1);
	*(((u8*)&out) + 3) = *(((u8*)&in) + 0);

	return out;
}