// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "RockAbilitySystemGlobals.generated.h"

struct FRockAttributeChangeInfo;
struct FRockAttributeSetInitter;
struct FRockAttributeInitializationKey;
/**
 * 
 */
UCLASS(Config=Game)
class ROCKMODULARGAMEPLAYABILITIES_API URockAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
public:
	static URockAbilitySystemGlobals& Get() { return *Cast<URockAbilitySystemGlobals>(IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals()); }

	TArray<FSoftObjectPath> GetCurveInitTables() const
	{
		return GetGlobalAttributeSetDefaultsTablePaths();
	}
	
	//~UAbilitySystemGlobals interface
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	virtual void AllocAttributeSetInitter() override;
	//~End of UAbilitySystemGlobals interface
	virtual TSharedPtr<FRockAttributeChangeInfo> AllocRockAttributeChangeInfo() const;

	virtual FRockAttributeSetInitter* GetRockAttributeSetInitter() const;
	virtual void ReloadAttributeDefaults() override;

	// ~ Begin FAttributeSetInitter
	//virtual void PreloadAttributeSetData(const TArray<UCurveTable*>& CurveData);
	virtual void InitAttributeSetDefaults(UAbilitySystemComponent* AbilitySystemComponent, const FRockAttributeInitializationKey& Key, int32 Level, bool bInitialInit) const;
	virtual void ApplyAttributeSetDefaults(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute& InAttribute, const FRockAttributeInitializationKey& Key, int32 Level) const;
	virtual TArray<float> GetAttributeSetValues(UClass* AttributeSetClass, FProperty* AttributeProperty, const FRockAttributeInitializationKey& Key) const;
	// ~ End FAttributeSetInitter Interface
};
