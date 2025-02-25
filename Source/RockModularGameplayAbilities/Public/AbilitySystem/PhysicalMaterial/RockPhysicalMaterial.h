// Copyright Broken Rock Studios LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Containers/RockGameplayTagFloatContainer.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "RockPhysicalMaterial.generated.h"


/**
 * URockPhysicalMaterial
 * A physical material that can be tagged with gameplay tags and have additional properties.
 * 
 * Example:  IRockAbilitySourceInterface::GetPhysicalMaterialAttenuation
 * A grenade might do more damage against something that is 'fragile' from an explosion
 * Possibly could be replaced with a generic TMap<GameplayTag, float> for the multiplier
 */
UCLASS()
class ROCKMODULARGAMEPLAYABILITIES_API URockPhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()

public:
	URockPhysicalMaterial(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// A container of gameplay tags that game code can use to reason about this physical material
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=PhysicalProperties)
	FGameplayTagContainer Tags;

	
	// NOTE: Seriously consider before adding more things here
	// Originally I was planning on adding float tags and even explosive damage modifiers
	// but later realized, it'd probably be better to leverage the TObjectPtr<UAEMaterialResponseMap> MaterialResponseMap as used in AEProjectile
	// So that individual objects can respectively add their own custom properties as needed. And swap out the behavior as needed.
	// We could even add a global default MaterialResponseMap data table as sorta the default fallback too
	// That way it'd be easier to swap out the behavior, instead of hardcoding it in per material asset. 

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	
	/**
	 * ExplosiveVulnerability represents how susceptible the material is to explosive damage.
	 * Higher values indicate the material is more likely to be damaged or shattered by explosions as opposed to direct impacts.
	 * This could be used in conjunction with Strength?
	 *
	 * Note: Be cautious to not go overboard with adding these types of values, since what about things like flame, ice, magic, we could start
	 * to introduce a limitless range of potential vulnerabilities or resistance based things.  They should likely be handled elsewhere.
	 * Note: Keep in mind that GeometryCollectionDamage is used for 'Chaos Destruction', which is different than this.
	 * Alternatively EB, had a UAEMaterialResponseMap : public UDataAsset, that has a mapping for PhysicalMaterial to various things
	 * With that approach, we could in theory have the ability to have different physical material responses based upon different dataassets
	 */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay",  meta=(ClampMin=0))
	//float ExplosiveVulnerability;

	
	// This would likely only be useful in 2 scenarios
	// 1. If we wanted some variables to only exist on some materials but not others. Cause otherwise we'd be better off hardcoding it in? 
	// 2. We wanted to generalize those floats to other games in a generic way. So different games could add as needed instead of hardcoding.
	// but I don't think I care about that right now. So we'll just hardcode things as needed.
	/**
	 * A container of gameplay tags and floats that game code can use to reason about this physical material
	 */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PhysicalProperties)
	//FRockGameplayTagFloatContainer GameplayTagFloatContainer;
	
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PhysicalProperties)
	//FRockGameplayTagStackContainer GameplayTagFloatContainer;

};