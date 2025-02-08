// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"

/**
 * 
 */
/** Explicit implementation of attribute set initter, relying on the existence and usage of discrete levels for data look-up (that is, CurveTable->Eval is not possible) */
// FAttributeSetInitterDiscreteLevels
struct ROCKMODULARGAMEPLAYABILITIES_API  FRockAttributeSetInitter : public FAttributeSetInitter
{

public:
	// ~ Begin FAttributeSetInitter
	virtual void PreloadAttributeSetData(const TArray<UCurveTable*>& CurveData) override;
	virtual void InitAttributeSetDefaults(UAbilitySystemComponent* AbilitySystemComponent, FName GroupName, int32 Level, bool bInitialInit) const override;
	virtual void ApplyAttributeDefault(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute& InAttribute, FName GroupName, int32 Level) const override;
	virtual TArray<float> GetAttributeSetValues(UClass* AttributeSetClass, FProperty* AttributeProperty, FName GroupName) const override;
	// ~ End FAttributeSetInitter Interface

private:
	bool IsSupportedProperty(FProperty* Property) const;

	
	struct FAttributeDefaultValueList
	{
		void AddPair(FProperty* InProperty, float InValue)
		{
			List.Add(FOffsetValuePair(InProperty, InValue));
		}

		struct FOffsetValuePair
		{
			FOffsetValuePair(FProperty* InProperty, float InValue)
			: Property(InProperty), Value(InValue) { }

			FProperty*	Property;
			float		Value;
		};

		TArray<FOffsetValuePair>	List;
	};

	struct FAttributeSetDefaults
	{
		TMap<TSubclassOf<UAttributeSet>, FAttributeDefaultValueList> DataMap;
	};

	struct FAttributeSetDefaultsCollection
	{
		TArray<FAttributeSetDefaults>		LevelData;
	};

	TMap<FName, FAttributeSetDefaultsCollection>	Defaults;
};

