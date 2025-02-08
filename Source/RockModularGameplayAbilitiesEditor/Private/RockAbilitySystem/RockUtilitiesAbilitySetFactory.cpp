// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#include "RockAbilitySystem/RockUtilitiesAbilitySetFactory.h"

#include "RockModularGameplayAbilities/Public/AbilitySystem/Abilities/RockAbilitySet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RockUtilitiesAbilitySetFactory)

URockUtilitiesAbilitySetFactory::URockUtilitiesAbilitySetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = URockAbilitySet::StaticClass();

	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* URockUtilitiesAbilitySetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	URockAbilitySet* Object = NewObject<URockAbilitySet>(InParent, Name, Flags);

	return Object;
}

FText URockUtilitiesAbilitySetFactory::GetDisplayName() const
{
	return FText::FromString("Gameplay Ability Set");
}