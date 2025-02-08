// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once

#include "AttributeSet.h"
#include "GameplayEffect.h"

#include "RockAttributeSet.generated.h"

struct FRockAttributeChangeInfo;
struct FGameplayEffectModCallbackData;
struct FGameplayAttribute;
class URockAbilitySystemComponent;

/**
 * This macro defines a set of helper functions for accessing and initializing attributes.
 *
 * The following example of the macro:
 *		ATTRIBUTE_ACCESSORS(URockHealthSet, Health)
 * will create the following functions:
 *		static FGameplayAttribute GetHealthAttribute();
 *		float GetHealth() const;
 *		void SetHealth(float NewVal);
 *		void InitHealth(float NewVal);
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


/** 
 * Delegate used to broadcast attribute events, some of these parameters may be null on clients: 
 * @param EffectInstigator	The original instigating actor for this event
 * @param EffectCauser		The physical actor that caused the change
 * @param EffectSpec		The full effect spec for this change
 * @param EffectMagnitude	The raw magnitude, this is before clamping
 * @param OldValue			The value of the attribute before it was changed
 * @param NewValue			The value after it was changed
*/
DECLARE_MULTICAST_DELEGATE_SixParams(FRockAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);
DECLARE_MULTICAST_DELEGATE(FRockSimpleAttributeEvent);
//DECLARE_MULTICAST_DELEGATE_ThreeParams(FRockSurvivalMaxAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/);


/**
 * URockAttributeSet
 *
 *	Base attribute set class for the project.
 */
UCLASS()
class ROCKMODULARGAMEPLAYABILITIES_API URockAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	URockAttributeSet();

	virtual UWorld* GetWorld() const override;
	URockAbilitySystemComponent* GetRockAbilitySystemComponent() const;

	virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
	{
	}
	// ~ Begin UAttributeSet
	virtual bool ShouldInitProperty(bool FirstInit, FProperty* PropertyToInit) const override;
	// ~ End UAttributeSet Interface
	
	virtual TSharedRef<FRockAttributeChangeInfo> GetAttributeChangeInfoFromModData(const FGameplayEffectModCallbackData& Data) const;

	/*
	 * Optional helper function
	 */
	void AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const;
};


USTRUCT(BlueprintType)
struct FRockAttributeInitializationKey
{
	GENERATED_BODY()

public:
	FName GetAttributeInitCategory() const;
	FName GetAttributeInitSubCategory() const;
	bool IsValid() const { return !AttributeInitCategory.IsNone() && !AttributeInitSubCategory.IsNone(); }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName AttributeInitCategory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName AttributeInitSubCategory;

#if WITH_EDITOR
	friend class FRockAttributeInitKeyCustomization;
#endif
};

//+GlobalAttributeSetDefaultsTableNames=/Game/AttributeTables/HeroAtributeData_Scaling.HeroAtributeData_Scaling
//+GlobalAttributeSetDefaultsTableNames=/Game/AttributeTables/AttributesBuilding.AttributesBuilding
//+GlobalAttributeSetDefaultsTableNames=/Game/AttributeTables/AIAttributeData_Scaling.AIAttributeData_Scaling
