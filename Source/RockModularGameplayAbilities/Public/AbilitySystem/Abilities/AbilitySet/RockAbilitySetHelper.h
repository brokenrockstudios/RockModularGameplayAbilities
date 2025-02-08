// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once
#include "GameplayTagContainer.h"

#include "RockAbilitySetHelper.generated.h"

class URockAbilitySystemComponent;
struct FActiveGameplayEffectHandle;
struct FGameplayAbilitySpecHandle;
class UAttributeSet;
class UGameplayEffect;
class URockGameplayAbility;

/**
 * FRockAbilitySet_GameplayAbility
 *
 *	Data used by the ability set to grant gameplay abilities.
 */
USTRUCT(BlueprintType)
struct ROCKMODULARGAMEPLAYABILITIES_API FRockAbilitySet_GameplayAbility
{
GENERATED_BODY()
public:

	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly, Category=Ability)
	TSubclassOf<URockGameplayAbility> Ability = nullptr;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly, Category=Ability)
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, Category=Ability, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


/**
 * FRockAbilitySet_GameplayEffect
 *
 *	Data used by the ability set to grant gameplay effects.
 */
USTRUCT(BlueprintType)
struct ROCKMODULARGAMEPLAYABILITIES_API FRockAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:

	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly, Category=GameplayEffect)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// Level of gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly, Category=GameplayEffect)
	float EffectLevel = 1.0f;
};

/**
 * FRockAbilitySet_AttributeSet
 *
 *	Data used by the ability set to grant attribute sets.
 */
USTRUCT(BlueprintType)
struct ROCKMODULARGAMEPLAYABILITIES_API FRockAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly, Category=AttributeSet)
	TSubclassOf<UAttributeSet> AttributeSet;

};

/**
 * FRockAbilitySet_GrantedHandles
 *
 *	Data used to store handles to what has been granted by the ability set.
 */
USTRUCT(BlueprintType)
struct ROCKMODULARGAMEPLAYABILITIES_API FRockAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	bool IsValid() const
	{
		return OwningASC.IsValid();// && HandleId != 0;
	}

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	void TakeFromAbilitySystem(URockAbilitySystemComponent* RockASC);

	friend class URockAbilitySet;
protected:

	// Handles to the granted abilities.
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Handles to the granted gameplay effects.
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// Pointers to the granted attribute sets
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;

	TWeakObjectPtr<URockAbilitySystemComponent> OwningASC = nullptr;
	// Is this needed?
	//int32 HandleId = 0;

private:
	void Reset()
	{
		//HandleId = 0;
		AbilitySpecHandles.Reset();
		GameplayEffectHandles.Reset();
		GrantedAttributeSets.Reset();
		OwningASC.Reset();
		
	}
};
