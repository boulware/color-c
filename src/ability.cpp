#include "ability.h"

// This function should be free to modify [ability] even if the parse fails,
// so anything using this should explicitly check the return value to make sure
// that [ability] is properly filled out (return true) -- and if it's not
// (return false), [ability] should be probably be discarded by the calling scope in most cases.
bool
ParseNextAsAbilityData(Buffer *buffer, Ability *ability)
{
	if(!buffer or !ability) return false;

	ability->tiers = CreatePermanentArray<AbilityTier>(c::max_ability_tier_count);

	bool valid_ability_data = true;
	char *initial = buffer->p;
	//Ability temp_ability = {};

	bool header_valid = ConfirmNextTokenAsIdentifier(buffer, "ability");
	if(!header_valid) valid_ability_data = false;

	Token name_token;
	bool is_valid_name = NextTokenAsDoubleQuotedString(buffer, &name_token);
	if(!is_valid_name) valid_ability_data = false;

	bool end_of_ability_data = false;
	int cur_tier = -1;
	size_t cur_effect_index = 0;
	while(valid_ability_data and !end_of_ability_data)
	{
		// Create checkpoint so that if it's an invalid token, we can revert the buffer to this location.
		char *before_token = buffer->p;

		Token token = NextToken(buffer);

		if(BufferBytesRemaining(*buffer) == 0 or TokenMatchesString(token, "ability"))
		{
			// reset buffer to before the previous token was fetched, since it's not part of the current
			// ability data
			buffer->p = before_token;
			end_of_ability_data = true;
		}
		else if(TokenMatchesString(token, "tier"))
		{
			++cur_tier;

			int parsed_tier;
			valid_ability_data = ParseNextAsS32(buffer, &parsed_tier);
			cur_effect_index = 0;

			if(parsed_tier != cur_tier)
			{
				valid_ability_data = false;
				if(c::verbose_error_logging) log(__FUNCTION__"() encountered invalid ability tier number. Got %d, expected %d", parsed_tier, cur_tier);
			}

			ability->tiers += {};
			ability->tiers[cur_tier].effects_ = CreatePermanentArray<Effect>(5);
		}
		else if(cur_tier >= 0)
		{
			if(TokenMatchesString(token, "requires"))
			{
				valid_ability_data = ParseNextAsTraitSet(buffer, &ability->tiers[cur_tier].required_traits);
			}
			else if(TokenMatchesString(token, "effect"))
			{
				if(cur_effect_index >= c::max_effect_count)
				{
					valid_ability_data = false;
					if(c::verbose_error_logging) log(__FUNCTION__"() encountered more effects than max_effect_count.");
				}

				token = NextToken(buffer);
				Effect effect = {};
				TargetClass tc = {};
				if(TokenMatchesString(token, "Damage"))
				{
					effect.type = EffectType::Damage;
					effect.params = AllocPerma(sizeof(EffectParams_Damage));

					EffectParams_Damage *params = (EffectParams_Damage*)effect.params;
					valid_ability_data = ParseNextAsTargetClass(buffer, &tc);
					valid_ability_data = ParseNextAsTraitSet(buffer, &params->amount);
				}
				else if(TokenMatchesString(token, "DamageIgnoreArmor"))
				{
					effect.type = EffectType::DamageIgnoreArmor;
					effect.params = AllocPerma(sizeof(EffectParams_DamageIgnoreArmor));

					EffectParams_DamageIgnoreArmor *params = (EffectParams_DamageIgnoreArmor*)effect.params;
					valid_ability_data = ParseNextAsTargetClass(buffer, &tc);
					valid_ability_data = ParseNextAsTraitSet(buffer, &params->amount);
				}
				else if(TokenMatchesString(token, "Restore"))
				{
					effect.type = EffectType::Restore;
					effect.params = AllocPerma(sizeof(EffectParams_Restore));

					EffectParams_Restore *params = (EffectParams_Restore*)effect.params;
					valid_ability_data = ParseNextAsTargetClass(buffer, &tc);
					valid_ability_data = ParseNextAsTraitSet(buffer, &params->amount);
				}
				else if(TokenMatchesString(token, "Gift"))
				{
					effect.type = EffectType::Gift;
					effect.params = AllocPerma(sizeof(EffectParams_Steal));

					EffectParams_Gift *params = (EffectParams_Gift*)effect.params;
					valid_ability_data = ParseNextAsTargetClass(buffer, &tc);
					valid_ability_data = ParseNextAsTraitSet(buffer, &params->amount);
				}
				else if(TokenMatchesString(token, "Steal"))
				{
					effect.type = EffectType::Steal;
					effect.params = AllocPerma(sizeof(EffectParams_Steal));

					EffectParams_Steal *params = (EffectParams_Steal*)effect.params;
					valid_ability_data = ParseNextAsTargetClass(buffer, &tc);
					valid_ability_data = ParseNextAsTraitSet(buffer, &params->amount);
				}

				ability->tiers[cur_tier].target_class = tc;
				ability->tiers[cur_tier].effects_[cur_effect_index] = effect;
				++cur_effect_index;
			}
		}
		else
		{
			valid_ability_data = false;
		}
	}

	if(valid_ability_data)
	{
		ability->name = StringFromToken(name_token, &memory::permanent_arena);

		return true;
	}
	else
	{
		buffer->p = initial;
		size_t number_of_bytes_to_print = m::Min(BufferBytesRemaining(*buffer), size_t(32));
		log("Encountered invalid ability data in buffer at address: %p (\"%.*s\")",
			buffer->p, number_of_bytes_to_print, buffer->p);

		return false;
	}
}

bool
LoadAbilityFile(const char *filename, Table<Ability> *table)
{
	if(!filename or !table) return false;

	Buffer file;
	bool load_success = platform->LoadFileIntoSizedBufferAndNullTerminate(filename, &file);
	if(!load_success) return false;

	size_t ability_count_loaded = 0;

	while(BufferBytesRemaining(file) > 0)
	{
		bool found_ability = SeekNextLineThatBeginsWith(&file, "ability");
		if(!found_ability) break;

		auto ability_id = CreateEntry(table);
		Ability *ability = GetAbilityFromId(ability_id);
		if(ability == nullptr) break;

		if(ParseNextAsAbilityData(&file, ability))
		{
			ability->init = true;
			++ability_count_loaded;
		}
		else
		{
			DeleteEntry(table, ability_id);
			continue;
		}
	}

	//log("Loaded %zu abilities from file: %s", ability_count_loaded, filename);
	FreeBuffer(&file);
	return true;
}

char *
GenerateAbilityTierText(AbilityTier tier)
{
	TIMED_BLOCK;

	const char *trait_names[c::trait_count] = {"vigor", "focus", "armor"};
	const char *trait_colors[c::trait_count] = {"red", "lt_blue", "gold"};

	Buffer buffer = {};
	buffer.data = ScratchString(c::max_effects_text_length+1);
	buffer.p = buffer.data;
	buffer.byte_count = c::max_effects_text_length;

	for(auto effect : tier.effects_)
	{ // For each effect in the ability tier
		if(effect.type == EffectType::NoEffect)
		{
			// Write nothing.
		}
		else if(effect.type == EffectType::Damage)
		{ // "Deal X vigor, Y focus, Z armor damage to [target]. "
			EffectParams_Damage *params = (EffectParams_Damage*)effect.params;
			bool at_least_one_positive_trait = false;
			for(s32 &trait : params->amount)
			{
				if(trait > 0)
				{
					at_least_one_positive_trait = true;
					break;
				}
			}
			// Write nothing if it doesn't actually deal any damage.
			if(!at_least_one_positive_trait) break;

			buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
								 "Deal ");

			bool at_least_one_trait_written = false; // Track this so we know whether to write a comma before a damage amount
			for(int i=0; i<c::trait_count; i++)
			{
				if(params->amount[i] > 0)
				{
					if(at_least_one_trait_written)
					{
						buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
											 "`reset`, ");
					}
					at_least_one_trait_written = true;

				buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
									 "`%s`%d %s", trait_colors[i], params->amount[i], trait_names[i]);
				}
			}

			buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
								 " damage `reset`to %s. ",
								 TargetClassToUserString(tier.target_class));
		}
		else if(effect.type == EffectType::DamageIgnoreArmor)
		{ // "Deal X vigor, Y focus, Z armor damage to [target]. Ignores armor. "
			EffectParams_DamageIgnoreArmor *params = (EffectParams_DamageIgnoreArmor*)effect.params;
			bool at_least_one_positive_trait = false;
			for(s32 &trait : params->amount)
			{
				if(trait > 0)
				{
					at_least_one_positive_trait = true;
					break;
				}
			}
			// Write nothing if it doesn't actually deal any damage.
			if(!at_least_one_positive_trait) break;

			buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
								 "Deal ");

			bool at_least_one_trait_written = false; // Track this so we know whether to write a comma before a damage amount
			for(int i=0; i<c::trait_count; i++)
			{
				if(params->amount[i] > 0)
				{
					if(at_least_one_trait_written)
					{
						buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
											 "`reset`, ");
					}
					at_least_one_trait_written = true;

				buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
									 "`%s`%d %s", trait_colors[i], params->amount[i], trait_names[i]);
				}
			}

			buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
								 " damage `reset`to %s. Ignores `yellow`armor`reset`. ",
								 TargetClassToUserString(tier.target_class));
		}
		else if(effect.type == EffectType::Restore)
		{ // "Restore X vigor, Y focus, Z armor to [target]. "
			EffectParams_Restore *params = (EffectParams_Restore*)effect.params;
			bool at_least_one_positive_trait = false;
			for(s32 &trait : params->amount)
			{
				if(trait > 0)
				{
					at_least_one_positive_trait = true;
					break;
				}
			}
			// Write nothing if it doesn't actually restore any amount.
			if(!at_least_one_positive_trait) break;

			buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
								 "Restore ");

			bool at_least_one_trait_written = false; // Track this so we know whether to write a comma before a damage amount
			for(int i=0; i<c::trait_count; i++)
			{
				if(params->amount[i] > 0)
				{
					if(at_least_one_trait_written)
					{
						buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
											 "`reset`, ");
					}
					at_least_one_trait_written = true;

				buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
									 "`%s`%d %s", trait_colors[i], params->amount[i], trait_names[i]);
				}
			}

			buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
								 " `reset`to %s. ",
								 TargetClassToUserString(tier.target_class));
		}
		else if(effect.type == EffectType::Gift)
		{ // "Gift X vigor, Y focus, Z armor to [target]."
			EffectParams_Gift *params = (EffectParams_Gift*)effect.params;
			bool at_least_one_positive_trait = false;
			for(s32 &trait : params->amount)
			{
				if(trait > 0)
				{
					at_least_one_positive_trait = true;
					break;
				}
			}
			// Write nothing if it doesn't actually restore any amount.
			if(!at_least_one_positive_trait) break;

			buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
								 "Gift ");

			bool at_least_one_trait_written = false; // Track this so we know whether to write a comma before a damage amount
			for(int i=0; i<c::trait_count; i++)
			{
				if(params->amount[i] > 0)
				{
					if(at_least_one_trait_written)
					{
						buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
											 "`reset`, ");
					}
					at_least_one_trait_written = true;

				buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
									 "`%s`%d %s", trait_colors[i], params->amount[i], trait_names[i]);
				}
			}

			buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
								 " `reset`to %s. ",
								 TargetClassToUserString(tier.target_class));
		}
		else if(effect.type == EffectType::Steal)
		{ // "Steal X vigor, Y focus, Z armor from [target]."
			EffectParams_Steal *params = (EffectParams_Steal*)effect.params;
			bool at_least_one_positive_trait = false;
			for(s32 &trait : params->amount)
			{
				if(trait > 0)
				{
					at_least_one_positive_trait = true;
					break;
				}
			}
			// Write nothing if it doesn't actually restore any amount.
			if(!at_least_one_positive_trait) break;

			buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
								 "Steal ");

			bool at_least_one_trait_written = false; // Track this so we know whether to write a comma before a damage amount
			for(int i=0; i<c::trait_count; i++)
			{
				if(params->amount[i] > 0)
				{
					if(at_least_one_trait_written)
					{
						buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
											 "`reset`, ");
					}
					at_least_one_trait_written = true;

				buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
									 "`%s`%d %s", trait_colors[i], params->amount[i], trait_names[i]);
				}
			}

			buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
								 " `reset`from %s. ",
								 TargetClassToUserString(tier.target_class));

		}
	}

	*buffer.p = '\0';

	return buffer.data;
}

Ability *
GetAbilityFromId(Id<Ability> id)
{
	return GetEntryFromId(g::ability_table, id);
}