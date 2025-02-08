// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once 

#include "UObject/Interface.h"

#include "RockAbilitySourceInterface.generated.h"

class UObject;
class UPhysicalMaterial;
struct FGameplayTagContainer;

/** Base interface for anything acting as an ability calculation source */
UINTERFACE()
class ROCKMODULARGAMEPLAYABILITIES_API URockAbilitySourceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Base interface for anything acting as an ability source
 */
class ROCKMODULARGAMEPLAYABILITIES_API IRockAbilitySourceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * Compute the multiplier for effect falloff with distance
	 * 
	 * @param Distance			Distance from source to target for ability calculations (distance bullet traveled for a gun, etc...)
	 * @param SourceTags		Aggregated Tags from the source
	 * @param TargetTags		Aggregated Tags currently on the target
	 * 
	 * @return Multiplier to apply to the base attribute value due to distance
	 */
	virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const = 0;

	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const = 0;

};
