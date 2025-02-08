// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RockAbilitySet.generated.h"


struct FRockAbilitySet_GrantedHandles;
class URockAbilitySystemComponent;
struct FRockAbilitySet_AttributeSet;
struct FRockAbilitySet_GameplayEffect;
struct FRockAbilitySet_GameplayAbility;
/**
 * URockAbilitySet
 *
 *	Non-mutable data asset used to grant gameplay abilities and gameplay effects.
 */
UCLASS(BlueprintType, Const)
class ROCKMODULARGAMEPLAYABILITIES_API URockAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	URockAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	FRockAbilitySet_GrantedHandles GiveAbilitySetTo(URockAbilitySystemComponent* RockASC, UObject* SourceObject) const;
	// Grants the ability set to the specified ability system component.
	// The returned handles can be used later to take away anything that was granted.
	void GiveToAbilitySystem(URockAbilitySystemComponent* RockASC, FRockAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

protected:

	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FRockAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FRockAbilitySet_GameplayEffect> GrantedGameplayEffects;

	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FRockAbilitySet_AttributeSet> GrantedAttributes;

	// Add Validation
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext &Context) const override;
#endif
	
};
