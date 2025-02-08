// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#include "AbilitySystem/Abilities/AbilitySet/RockAbilitySetHelper.h"

#include "AbilitySystem/Components/RockAbilitySystemComponent.h"

inline void FRockAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

inline void FRockAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

inline void FRockAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

void FRockAbilitySet_GrantedHandles::TakeFromAbilitySystem(URockAbilitySystemComponent* RockASC)
{
	check(RockASC);
	if (!RockASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			if(const FGameplayAbilitySpec* AbilitySpec = RockASC->FindAbilitySpecFromHandle(Handle))
			{
				UGameplayAbility* Instance = AbilitySpec->GetPrimaryInstance();
				if(!Instance) {
					Instance = AbilitySpec->Ability;
				}
				if(URockGameplayAbility* Ability = Cast<URockGameplayAbility>(Instance)) {
					Ability->OnRemoveAbility(RockASC->AbilityActorInfo.Get(), *AbilitySpec);
				}
			}
			RockASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			RockASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		RockASC->RemoveSpawnedAttribute(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}
