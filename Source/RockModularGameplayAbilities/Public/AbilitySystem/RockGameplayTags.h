
#pragma once

#include "NativeGameplayTags.h"

namespace RockGameplayTags
{
ROCKMODULARGAMEPLAYABILITIES_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

// Declare all the custom native tags that Rock Modular GameplayAbilities will use
ROCKMODULARGAMEPLAYABILITIES_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
ROCKMODULARGAMEPLAYABILITIES_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
ROCKMODULARGAMEPLAYABILITIES_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
ROCKMODULARGAMEPLAYABILITIES_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
ROCKMODULARGAMEPLAYABILITIES_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
ROCKMODULARGAMEPLAYABILITIES_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
ROCKMODULARGAMEPLAYABILITIES_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);

// Is this too 'specific' for this plugin?
ROCKMODULARGAMEPLAYABILITIES_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	

}
