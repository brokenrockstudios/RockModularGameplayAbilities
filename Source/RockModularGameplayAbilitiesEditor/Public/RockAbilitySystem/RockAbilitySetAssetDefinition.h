// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once

#include "CoreMinimal.h"
#include "AssetDefinitionDefault.h"
#include "RockAbilitySetAssetDefinition.generated.h"

/**
 * 
 */
UCLASS()
class ROCKMODULARGAMEPLAYABILITIESEDITOR_API URockAbilitySetAssetDefinition : public UAssetDefinitionDefault
{
	GENERATED_BODY()

public:
	// UAssetDefinition Begin
	virtual FText GetObjectDisplayNameText(UObject* Object) const override;
	virtual FText GetAssetDisplayName() const override;
	// virtual FText GetAssetDisplayName(const FAssetData& AssetData) override; 
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual FLinearColor GetAssetColor() const override;
	// virtual FText GetAssetDescription(const FAssetData& AssetData) override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	// ~UAssetDefinition End

};
