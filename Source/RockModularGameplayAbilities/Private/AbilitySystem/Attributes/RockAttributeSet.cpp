// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#include "AbilitySystem/Attributes/RockAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "AbilitySystem/Attributes/RockAttributeChangeInfo.h"
#include "AbilitySystem/Components/RockAbilitySystemComponent.h"
#include "AbilitySystem/Global/RockAbilitySystemGlobals.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RockAttributeSet)

URockAttributeSet::URockAttributeSet()
{
}

UWorld* URockAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}


URockAbilitySystemComponent* URockAttributeSet::GetRockAbilitySystemComponent() const
{
	return Cast<URockAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

bool URockAttributeSet::ShouldInitProperty(bool FirstInit, FProperty* PropertyToInit) const
{
	// This should be overridden in the derived class to handle the initialization of properties that depend on other properties
	// For example, in the UCoreAttributeSet, the Vitality attribute is used to calculate the VitalityHealthBonus attribute.
	// Namely… all the attributes of mine that depend on other attributes (*VitalityHealthBonus *etc.)
	// can’t be initialized with the character level. Even moreso since it can happen that *VitalityHealthBonus *is
	// assigned before Vitality. Thus, for the UCoreAttributeSet, the ShouldInitProperty() function looks like this:
	// 	return PropertyToInit != VitalityHealthBonusAttribute().GetUProperty() &&
	// 		PropertyToInit != HealthRegenPerVitalityAttribute().GetUProperty();
	// }
	
	return Super::ShouldInitProperty(FirstInit, PropertyToInit);
}

TSharedRef<FRockAttributeChangeInfo> URockAttributeSet::GetAttributeChangeInfoFromModData(const FGameplayEffectModCallbackData& Data) const
{
	// Allocate a new instance of FRockAttributeChangeInfo.
	const TSharedPtr<FRockAttributeChangeInfo> AttributeChangeInfo = URockAbilitySystemGlobals::Get().AllocRockAttributeChangeInfo();
	
	AttributeChangeInfo->Context = Data.EffectSpec.GetContext();
	Data.EffectSpec.GetAllAssetTags(AttributeChangeInfo->SpecAssetTags);
	
	// Set SourceASC (and note: consider how to handle non-Rock ASCs if needed)
	AttributeChangeInfo->SourceASC = Cast<URockAbilitySystemComponent>(AttributeChangeInfo->Context.GetOriginalInstigatorAbilitySystemComponent());

	if (AttributeChangeInfo->SourceASC && AttributeChangeInfo->SourceASC->AbilityActorInfo.IsValid())
	{
		// Prefer the effect causer if available; otherwise use the avatar actor.
		if (AttributeChangeInfo->Context.GetEffectCauser())
		{
			AttributeChangeInfo->SourceActor = AttributeChangeInfo->Context.GetEffectCauser();
		}
		else if (AttributeChangeInfo->SourceASC->AbilityActorInfo->AvatarActor.IsValid())
		{
			AttributeChangeInfo->SourceActor = AttributeChangeInfo->SourceASC->AbilityActorInfo->AvatarActor.Get();
		}
		else
		{
			AttributeChangeInfo->SourceActor = nullptr;
		}
		
		AttributeChangeInfo->SourceController =AttributeChangeInfo->SourceASC->AbilityActorInfo->PlayerController.IsValid()
			? AttributeChangeInfo->SourceASC->AbilityActorInfo->PlayerController.Get()
			: nullptr;
	}

	if(Data.Target.AbilityActorInfo.IsValid())
	{
		AttributeChangeInfo->TargetActor = Data.Target.AbilityActorInfo->AvatarActor.IsValid()
			? Data.Target.AbilityActorInfo->AvatarActor.Get()
			: nullptr;
		AttributeChangeInfo->TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
	}
	else
	{
		AttributeChangeInfo->TargetActor = nullptr;
		AttributeChangeInfo->TargetController = nullptr;
	}
	

	// Gameplay Tags
	AttributeChangeInfo->SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	AttributeChangeInfo->TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
	
	AttributeChangeInfo->SourceObject = Data.EffectSpec.GetEffectContext().GetSourceObject();

	// Set the delta magnitude based on the modifier operation.
	AttributeChangeInfo->DeltaMagnitude = 0.0f;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Additive)
	{
		AttributeChangeInfo->DeltaMagnitude = Data.EvaluatedData.Magnitude;
	}
	// TODO: Handle other modifier operations (e.g., multiplicative) if needed.
	
	return AttributeChangeInfo.ToSharedRef();
}

void URockAttributeSet::AdjustAttributeForMaxChange(	const FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}
	
	// cache current values
	const float currentMaxValue = MaxAttribute.GetCurrentValue();
	const float currentValue = AffectedAttribute.GetCurrentValue();
	
	if (FMath::IsNearlyEqual(currentMaxValue, NewMaxValue)  || currentMaxValue == 0.f)
	{
		return;
	}
	// Change current value to maintain the current Value / Max percent
	const float Ratio = currentValue / currentMaxValue;
	
	const float NewValue = FMath::RoundToFloat(Ratio *  NewMaxValue);
	//- currentValue;
	// LOG

	ASC->ApplyModToAttribute(AffectedAttributeProperty, EGameplayModOp::Override, NewValue);
	// I've seen in some instances, people using the Unsafe and Additive versions of this function. Consider using those if needed.
	//	ASC->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
}

FName FRockAttributeInitializationKey::GetAttributeInitCategory() const
{
	return AttributeInitCategory;
}

FName FRockAttributeInitializationKey::GetAttributeInitSubCategory() const
{
	return AttributeInitSubCategory;
}
