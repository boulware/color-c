#include "ability.h"

bool
ParseNextAsAbilityData(Buffer *buffer, Ability *ability)
{
	if(!buffer or !ability) return false;

	bool valid_ability_data = true;
	char *initial = buffer->p;
	Ability temp_ability = {};

	bool header_valid = ConfirmNextToken(buffer, "ability");
	if(!header_valid) valid_ability_data = false;

	Token name_token;
	bool is_valid_name = NextTokenAsDoubleQuotedString(buffer, &name_token);
	if(!is_valid_name) valid_ability_data = false;

	bool end_of_ability_data = false;
	int cur_tier = 0;
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
		// @note: Better useage code syntax might be something like if(TokenMatches(token, "level")) ?
		// CompareStrings() is actually slightly different from what we'd want here, but it works similarly
		// in most cases.
		else if(TokenMatchesString(token, "tier"))
		{
			valid_ability_data = ParseNextAsS32(buffer, &cur_tier);
			cur_effect_index = 0;
			if(cur_tier < 0 or cur_tier >= c::max_ability_tier_count)
			{
				valid_ability_data = false;
				if(c::verbose_error_logging) log(__FUNCTION__"() encountered invalid ability tier: %d", cur_tier);
			}
		}
		else if(TokenMatchesString(token, "requires"))
		{
			valid_ability_data = ParseNextAsTraitSet(buffer, &temp_ability.tiers[cur_tier].required_traits);
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
			if(TokenMatchesString(token, "Damage"))
			{
				effect.type = EffectType::Damage;
				effect.params = AllocPerma(sizeof(EffectParams_Damage));

				EffectParams_Damage *params = (EffectParams_Damage*)effect.params;
				valid_ability_data = ParseNextAsTargetClass(buffer, &effect.target_class);
				valid_ability_data = ParseNextAsTraitSet(buffer, &params->amount);
			}
			else if(TokenMatchesString(token, "DamageIgnoreArmor"))
			{
				effect.type = EffectType::DamageIgnoreArmor;
				effect.params = AllocPerma(sizeof(EffectParams_DamageIgnoreArmor));

				EffectParams_DamageIgnoreArmor *params = (EffectParams_DamageIgnoreArmor*)effect.params;
				valid_ability_data = ParseNextAsTargetClass(buffer, &effect.target_class);
				valid_ability_data = ParseNextAsTraitSet(buffer, &params->amount);
			}
			else if(TokenMatchesString(token, "Restore"))
			{
				effect.type = EffectType::Restore;
				effect.params = AllocPerma(sizeof(EffectParams_Restore));

				EffectParams_Restore *params = (EffectParams_Restore*)effect.params;
				valid_ability_data = ParseNextAsTargetClass(buffer, &effect.target_class);
				valid_ability_data = ParseNextAsTraitSet(buffer, &params->amount);
			}
			else if(TokenMatchesString(token, "Gift"))
			{
				effect.type = EffectType::Gift;
				effect.params = AllocPerma(sizeof(EffectParams_Steal));

				EffectParams_Gift *params = (EffectParams_Gift*)effect.params;
				valid_ability_data = ParseNextAsTargetClass(buffer, &effect.target_class);
				valid_ability_data = ParseNextAsTraitSet(buffer, &params->amount);
			}
			else if(TokenMatchesString(token, "Steal"))
			{
				effect.type = EffectType::Steal;
				effect.params = AllocPerma(sizeof(EffectParams_Steal));

				EffectParams_Steal *params = (EffectParams_Steal*)effect.params;
				valid_ability_data = ParseNextAsTargetClass(buffer, &effect.target_class);
				valid_ability_data = ParseNextAsTraitSet(buffer, &params->amount);
			}

			temp_ability.tiers[cur_tier].effects[cur_effect_index++] = effect;
		}
		else
		{
			valid_ability_data = false;
		}
	}

	if(valid_ability_data)
	{
		*ability = temp_ability;
		CopyString(ability->name, name_token.start, m::Min(sizeof(ability->name), name_token.length+1));

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
LoadAbilityFile(const char *filename, DataTable *table)
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

		if(DataTableEntriesRemaining(*table) >= 1)
		{
			Ability temp_ability = {};
			if(ParseNextAsAbilityData(&file, &temp_ability))
			{
				Ability *ability = (Ability*)CreateEntry(table);
				*ability = temp_ability;
				ability->init = true;
				++ability_count_loaded;
			}
			else
			{
				continue;
			}
		}
	}

	//log("Loaded %zu abilities from file: %s", ability_count_loaded, filename);
	FreeBuffer(&file);
	return true;
}

char *
GenerateAbilityTierText(const AbilityTier *tier)
{
	TIMED_BLOCK;

	Buffer buffer = {};
	buffer.data = ScratchString(c::max_effects_text_length+1);
	buffer.p = buffer.data;
	buffer.byte_count = c::max_effects_text_length;

	for(const Effect &effect : tier->effects)
	{ // For each effect in the ability tier
		if(effect.type == EffectType::NoEffect)
		{
			// Write nothing.
		}
		if(effect.type == EffectType::Damage)
		{ // "Deals X vigor, Y focus, Z armor damage. "
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
								 "Deals ");

			bool at_least_one_trait_written = false; // Track this so we know whether to write a comma before a damage amount
			const char *trait_names[c::trait_count] = {"vigor", "focus", "armor"};
			for(int i=0; i<c::trait_count; i++)
			{
				if(params->amount[i] > 0)
				{
					if(at_least_one_trait_written)
					{
						buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
											 ", ");
					}
					at_least_one_trait_written = true;

				buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
									 "%d %s", params->amount[i], trait_names[i]);
				}
			}

			buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
								 " damage. ");
		}
		if(effect.type == EffectType::DamageIgnoreArmor)
		{ // "Deals X vigor, Y focus, Z armor damage. Ignores armor. "
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
								 "Deals ");

			bool at_least_one_trait_written = false; // Track this so we know whether to write a comma before a damage amount
			const char *trait_names[c::trait_count] = {"vigor", "focus", "armor"};
			for(int i=0; i<c::trait_count; i++)
			{
				if(params->amount[i] > 0)
				{
					if(at_least_one_trait_written)
					{
						buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
											 ", ");
					}
					at_least_one_trait_written = true;

				buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
									 "%d %s", params->amount[i], trait_names[i]);
				}
			}

			buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
								 " damage. Ignores armor. ");
		}
		if(effect.type == EffectType::Restore)
		{ // "Restores X vigor, Y focus, Z armor. "
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
								 "Restores ");

			bool at_least_one_trait_written = false; // Track this so we know whether to write a comma before a damage amount
			const char *trait_names[c::trait_count] = {"vigor", "focus", "armor"};
			for(int i=0; i<c::trait_count; i++)
			{
				if(params->amount[i] > 0)
				{
					if(at_least_one_trait_written)
					{
						buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
											 ", ");
					}
					at_least_one_trait_written = true;

				buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
									 "%d %s", params->amount[i], trait_names[i]);
				}
			}

			buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
								 ". ");
		}
		if(effect.type == EffectType::Gift)
		{ // "Gifts X vigor, Y focus, Z armor."
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
								 "Gifts ");

			bool at_least_one_trait_written = false; // Track this so we know whether to write a comma before a damage amount
			const char *trait_names[c::trait_count] = {"vigor", "focus", "armor"};
			for(int i=0; i<c::trait_count; i++)
			{
				if(params->amount[i] > 0)
				{
					if(at_least_one_trait_written)
					{
						buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
											 ", ");
					}
					at_least_one_trait_written = true;

				buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
									 "%d %s", params->amount[i], trait_names[i]);
				}
			}

			buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
								 ". ");
		}
		if(effect.type == EffectType::Steal)
		{ // "Steals X vigor, Y focus, Z armor."
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
								 "Steals ");

			bool at_least_one_trait_written = false; // Track this so we know whether to write a comma before a damage amount
			const char *trait_names[c::trait_count] = {"vigor", "focus", "armor"};
			for(int i=0; i<c::trait_count; i++)
			{
				if(params->amount[i] > 0)
				{
					if(at_least_one_trait_written)
					{
						buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
											 ", ");
					}
					at_least_one_trait_written = true;

				buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
									 "%d %s", params->amount[i], trait_names[i]);
				}
			}

			buffer.p += snprintf(buffer.p, BufferBytesRemaining(buffer),
								 ". ");

		}
	}

	*buffer.p = '\0';

	return buffer.data;
}