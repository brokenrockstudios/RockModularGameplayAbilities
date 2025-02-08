// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"

#include "RockAttributeChangeInfo.generated.h"

class URockAbilitySystemComponent;

/**
 * @brief Aggregates essential details extracted from a Gameplay Effect modification callback.
 *
 * This struct encapsulates the key context and state information associated with an attribute change event.
 * It includes the effect context, source/target actors and their associated Ability System Components (ASCs)
 * and controllers, as well as gameplay and asset tags that might influence effect behavior.
 */
USTRUCT(BlueprintType)
struct ROCKMODULARGAMEPLAYABILITIES_API FRockAttributeChangeInfo
{
	GENERATED_BODY()

public:
	/**
	* The context of the gameplay effect, which includes information such as the instigator,
	* effect causer, and source object.
	*/
	UPROPERTY()
	FGameplayEffectContextHandle Context;

	/**
	 * The actor that initiated or caused this attribute change.
	 */
	UPROPERTY()
	TObjectPtr<AActor> SourceActor = nullptr;

	/**
	* The actor that is the target of this attribute change.
	*/
	UPROPERTY()
	TObjectPtr<AActor> TargetActor = nullptr;
	
	/**
	 * The Ability System Component (ASC) associated with the source of this attribute change.
	 */
	UPROPERTY()
	TObjectPtr<URockAbilitySystemComponent> SourceASC = nullptr;

	/**
	* The Ability System Component (ASC) associated with the target of this attribute change.
	* (This may be assigned elsewhere if needed.)
	*/
	UPROPERTY()
	TObjectPtr<URockAbilitySystemComponent> TargetASC = nullptr;

	/**
	 * The controller for the source actor.
	 */
	UPROPERTY()
	TObjectPtr<AController> SourceController = nullptr;

	/**
	* The controller for the target actor.
	*/
	UPROPERTY()
	TObjectPtr<AController> TargetController = nullptr;
	
	/**
	* Gameplay tags associated with the source actor or effect.
	*/
	UPROPERTY()
	FGameplayTagContainer SourceTags;

	/**
	* Asset tags extracted from the effect specification.
	* These tags may be used to provide additional metadata or to drive effect-specific logic.
	*/
	UPROPERTY()
	FGameplayTagContainer SpecAssetTags;

	/**
	 * Gameplay tags associated with the target actor or effect.
	 */
	UPROPERTY()
	FGameplayTagContainer TargetTags;

	/**
	 * An optional reference to an object defined as the source in the effect context.
	 */
	UPROPERTY()
	TObjectPtr<UObject> SourceObject = nullptr;

	/**
	 * The magnitude of the attribute change.
	 * For additive operations, this represents the delta applied; other operation types might not use this value.
	 */
	UPROPERTY()
	float DeltaMagnitude = 0.0f;
};