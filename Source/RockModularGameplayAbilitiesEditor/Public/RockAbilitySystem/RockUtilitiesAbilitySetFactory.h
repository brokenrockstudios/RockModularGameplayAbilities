// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "RockUtilitiesAbilitySetFactory.generated.h"

UCLASS(hidecategories = Object, MinimalAPI)
class URockUtilitiesAbilitySetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
	//~ Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual FText GetDisplayName() const override;

	virtual bool ShouldShowInNewMenu() const override
	{
		return true;
	}

	//~ End UFactory Interface	
};