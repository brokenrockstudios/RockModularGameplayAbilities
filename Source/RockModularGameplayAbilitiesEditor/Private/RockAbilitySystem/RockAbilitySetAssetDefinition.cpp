// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#include "RockAbilitySystem/RockAbilitySetAssetDefinition.h"

#include "AbilitySystem/Abilities/RockAbilitySet.h"


#define LOCTEXT_NAMESPACE "RockGASUtilities_Editor"

FText URockAbilitySetAssetDefinition::GetAssetDisplayName() const
{
	return LOCTEXT("AssetTypeActions_RockGameplayAbilitySet", "Gameplay Ability Set");
}

FText URockAbilitySetAssetDefinition::GetObjectDisplayNameText(UObject* Object) const
{
	return Super::GetObjectDisplayNameText(Object);
}

TSoftClassPtr<UObject> URockAbilitySetAssetDefinition::GetAssetClass() const
{
	return URockAbilitySet::StaticClass();
}

FLinearColor URockAbilitySetAssetDefinition::GetAssetColor() const
{
	return FLinearColor(FColor(21, 101, 34));
}

TConstArrayView<FAssetCategoryPath> URockAbilitySetAssetDefinition::GetAssetCategories() const
{
	static const auto Categories = {FAssetCategoryPath(LOCTEXT("RockCategory", "Rock"), LOCTEXT("RockCategorySubAbility", "Ability"))};
	return Categories;
}

#undef LOCTEXT_NAMESPACE
