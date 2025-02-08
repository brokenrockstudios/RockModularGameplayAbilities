// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"

#include "RockGameplayEffectContext.generated.h"

class IRockAbilitySourceInterface;
/**
 * 
 */
USTRUCT(BlueprintType)
struct ROCKMODULARGAMEPLAYABILITIES_API FRockGameplayEffectContext : public FGameplayEffectContext
{
public:
	GENERATED_BODY()
	
	FRockGameplayEffectContext() : FGameplayEffectContext() {}
	FRockGameplayEffectContext(const FGameplayEffectContext& Other) : FGameplayEffectContext(Other) {}
	
	/** Returns the wrapped FRockGameplayEffectContext from the handle, or nullptr if it doesn't exist or is the wrong type */
	static FRockGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

	/** Sets the object used as the ability source */
	void SetAbilitySource(const IRockAbilitySourceInterface* InObject, float InSourceLevel);

	/** Returns the ability source interface associated with the source object. Only valid on the authority. */
	const IRockAbilitySourceInterface* GetAbilitySource() const;

	virtual FGameplayEffectContext* Duplicate() const override
	{
		FRockGameplayEffectContext* NewContext = new FRockGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FRockGameplayEffectContext::StaticStruct();
	}

	/** Overridden to serialize new fields */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	/** Returns the physical material from the hit result if there is one */
	const UPhysicalMaterial* GetPhysicalMaterial() const;
	
	/** ID to allow the identification of multiple instances that were part of the same initiator (e.g. multiple bullets from same CartridgeID */
	UPROPERTY()
	int32 ID = -1;
	
protected:
	/** Ability Source object (should implement IRockAbilitySourceInterface). NOT replicated currently */
	UPROPERTY()
	TWeakObjectPtr<const UObject> AbilitySourceObject;
};

template<>
struct TStructOpsTypeTraits<FRockGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FRockGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

