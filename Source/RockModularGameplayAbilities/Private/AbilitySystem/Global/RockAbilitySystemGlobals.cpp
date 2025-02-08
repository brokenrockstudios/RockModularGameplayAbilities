// Copyright Broken Rock Studios LLC. All Rights Reserved.


#include "AbilitySystem/Global/RockAbilitySystemGlobals.h"

#include "AbilitySystem/RockGameplayEffectContext.h"
#include "AbilitySystem/Attributes/RockAttributeChangeInfo.h"
#include "AbilitySystem/Attributes/RockAttributeSet.h"
#include "AbilitySystem/Attributes/RockAttributeSetInitter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RockAbilitySystemGlobals)

FGameplayEffectContext* URockAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FRockGameplayEffectContext();
}

TSharedPtr<FRockAttributeChangeInfo> URockAbilitySystemGlobals::AllocRockAttributeChangeInfo() const
{
	return MakeShared<FRockAttributeChangeInfo>();
}

void URockAbilitySystemGlobals::AllocAttributeSetInitter()
{
	GlobalAttributeSetInitter = MakeShared<FRockAttributeSetInitter>();
}

FRockAttributeSetInitter* URockAbilitySystemGlobals::GetRockAttributeSetInitter() const
{
	return static_cast<FRockAttributeSetInitter*>(GetAttributeSetInitter());

}

void URockAbilitySystemGlobals::InitAttributeSetDefaults(
	UAbilitySystemComponent* AbilitySystemComponent, const FRockAttributeInitializationKey& Key, int32 Level, bool bInitialInit) const
{
	if (ensure(Key.IsValid()))
	{
		FName GroupName = Key.GetAttributeInitCategory();
		if (!Key.GetAttributeInitSubCategory().IsNone())
		{
			GroupName = FName(*FString::Printf(TEXT("%s.%s"), *Key.GetAttributeInitCategory().ToString(), *Key.GetAttributeInitSubCategory().ToString()));
		}
		GetRockAttributeSetInitter()->InitAttributeSetDefaults(AbilitySystemComponent, GroupName, Level, bInitialInit);
	}
}

void URockAbilitySystemGlobals::ApplyAttributeSetDefaults(
	UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute& InAttribute, const FRockAttributeInitializationKey& Key, int32 Level) const
{
	if (ensure(Key.IsValid()))
	{
		FName GroupName = Key.GetAttributeInitCategory();
		if (!Key.GetAttributeInitSubCategory().IsNone())
		{
			GroupName = FName(*FString::Printf(TEXT("%s.%s"), *Key.GetAttributeInitCategory().ToString(), *Key.GetAttributeInitSubCategory().ToString()));
		}
		GetRockAttributeSetInitter()->ApplyAttributeDefault(AbilitySystemComponent, InAttribute, GroupName, Level);
	}
}

TArray<float> URockAbilitySystemGlobals::GetAttributeSetValues(
	UClass* AttributeSetClass, FProperty* AttributeProperty, const FRockAttributeInitializationKey& Key) const
{
	if (ensure(Key.IsValid()))
	{
		FName GroupName = Key.GetAttributeInitCategory();
		if (!Key.GetAttributeInitSubCategory().IsNone())
		{
			GroupName = FName(*FString::Printf(TEXT("%s.%s"), *Key.GetAttributeInitCategory().ToString(), *Key.GetAttributeInitSubCategory().ToString()));
		}
		return GetRockAttributeSetInitter()->GetAttributeSetValues(AttributeSetClass, AttributeProperty, GroupName);
	}
	return {};
}

void URockAbilitySystemGlobals::ReloadAttributeDefaults()
{
	Super::ReloadAttributeDefaults();
}
