// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#include "AbilitySystem/RockGameplayEffectContext.h"

#include "AbilitySystem/RockAbilitySourceInterface.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
#include "Serialization/GameplayEffectContextNetSerializer.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(RockGameplayEffectContext)


FRockGameplayEffectContext* FRockGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FRockGameplayEffectContext::StaticStruct()))
	{
		return static_cast<FRockGameplayEffectContext*>(BaseEffectContext);
	}

	return nullptr;
}

void FRockGameplayEffectContext::SetAbilitySource(const IRockAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const IRockAbilitySourceInterface* FRockGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IRockAbilitySourceInterface>(AbilitySourceObject.Get());
}

bool FRockGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	// First, call the base class serializer.
	if (!FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}

	// --- New: Serialize CartridgeID conditionally ---
	// Only serialize CartridgeID if we’re in the pre-activation phase.
	//if (!bPostActivation)
	{
		//Ar << CartridgeID;
	}
	// If loading in post-activation, set CartridgeID to a default value.
	//else if (Ar.IsLoading())
	{
		//CartridgeID = 0; // or any appropriate default value
	}
	
	// Not serialized for post-activation use:
	// ID, formerly known as CartridgeID
    
	bOutSuccess = true;
	return true;
}

#if UE_WITH_IRIS
namespace UE::Net
{
// Forward to FGameplayEffectContextNetSerializer
// Note: If FRockGameplayEffectContext::NetSerialize() is modified, a custom NetSerializer must be implemented as the current fallback will no longer be sufficient.
UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(RockGameplayEffectContext, FGameplayEffectContextNetSerializer);
}
#endif

const UPhysicalMaterial* FRockGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}
