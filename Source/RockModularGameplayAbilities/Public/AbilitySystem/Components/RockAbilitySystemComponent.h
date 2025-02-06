// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/RockActivationGroup.h"
#include "AbilitySystem/Abilities/RockGameplayAbility.h"
#include "RockAbilitySystemComponent.generated.h"


class URockAbilityTagRelationshipMapping;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ROCKMODULARGAMEPLAYABILITIES_API URockAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URockAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	typedef TFunctionRef<bool(const URockGameplayAbility* RockAbility, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
	void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);
	
	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface
	static URockAbilitySystemComponent* GetAbilitySystemComponentFromActor(const AActor* Actor, bool bSearchComponents);

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	void TryActivateAbilitiesOnSpawn();

	

protected:

public:

	bool IsActivationGroupBlocked(ERockAbilityActivationGroup Group) const;
	void AddAbilityToActivationGroup(ERockAbilityActivationGroup Group, URockGameplayAbility* RockAbility);
	void RemoveAbilityFromActivationGroup(ERockAbilityActivationGroup Group, URockGameplayAbility* RockAbility);
	void CancelActivationGroupAbilities(ERockAbilityActivationGroup Group, URockGameplayAbility* IgnoreRockAbility, bool bReplicateCancelAbility);

	/** Gets the ability target data associated with the given ability handle and activation info */
	void GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle);

	/** Sets the current tag relationship mapping, if null it will clear it out */
	void SetTagRelationshipMapping(URockAbilityTagRelationshipMapping* NewMapping);
	
	/** Looks at ability tags and gathers additional required and blocking tags */
	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;

	
protected:

	// If set, this table is used to look up tag relationships for activate and cancel
	UPROPERTY()
	TObjectPtr<URockAbilityTagRelationshipMapping> TagRelationshipMapping;
	
	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	// Number of abilities running in each activation group.
	int32 ActivationGroupCounts[static_cast<uint8>(ERockAbilityActivationGroup::MAX)];

};
