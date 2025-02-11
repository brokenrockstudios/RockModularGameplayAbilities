// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#include "AbilitySystem/Abilities/RockAbilitySet.h"

#include "AbilitySystem/Abilities/AbilitySet/RockAbilitySetHelper.h"
#include "AbilitySystem/Components/RockAbilitySystemComponent.h"
#include "Logging/RockLogging.h"

#define LOCTEXT_NAMESPACE "RockAbilitySet"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(RockAbilitySet)

URockAbilitySet::URockAbilitySet(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FRockAbilitySet_GrantedHandles URockAbilitySet::GiveAbilitySetTo(URockAbilitySystemComponent* RockASC, UObject* SourceObject) const
{
	FRockAbilitySet_GrantedHandles OutHandle;
	GiveToAbilitySystem(RockASC, &OutHandle, SourceObject);
	return OutHandle;
}

void URockAbilitySet::GiveToAbilitySystem(
	URockAbilitySystemComponent* RockASC, FRockAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(RockASC);

	if (!RockASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FRockAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogRockAbilitySystem, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		URockGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<URockGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = RockASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FRockAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogRockAbilitySystem, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex, *GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = RockASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, RockASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	// Grant the attribute sets.
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FRockAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogRockAbilitySystem, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex, *GetNameSafe(this));
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(RockASC->GetOwner(), SetToGrant.AttributeSet);
		RockASC->AddAttributeSetSubobject(NewSet);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}

#if WITH_EDITOR
EDataValidationResult URockAbilitySet::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	for (int32 Index = 0; Index < GrantedGameplayAbilities.Num(); ++Index)
	{
		const FRockAbilitySet_GameplayAbility& Ability = GrantedGameplayAbilities[Index];
		if (Ability.Ability == nullptr)
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("MissingGameplayAbility", "Null entry at index {0} in GrantedGameplayAbilities"), FText::AsNumber(Index)));
		}
	}
	for (int32 Index = 0; Index < GrantedGameplayEffects.Num(); ++Index)
	{
		const FRockAbilitySet_GameplayEffect& Effect = GrantedGameplayEffects[Index];
		if (Effect.GameplayEffect == nullptr)
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("MissingGameplayEffect", "Null entry at index {0} in GrantedGameplayEffects"), FText::AsNumber(Index)));
		}
	}
	for (int32 Index = 0; Index < GrantedAttributes.Num(); ++Index)
	{
		const FRockAbilitySet_AttributeSet& AttributeSet = GrantedAttributes[Index];
		if (AttributeSet.AttributeSet == nullptr)
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("MissingAttributeSet", "Null entry at index {0} in GrantedAttributes"), FText::AsNumber(Index)));
		}
	}
	
	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE