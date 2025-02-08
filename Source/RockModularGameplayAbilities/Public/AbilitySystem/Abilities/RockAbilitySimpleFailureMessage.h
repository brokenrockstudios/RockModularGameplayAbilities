// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

#include "RockAbilitySimpleFailureMessage.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_ABILITY_SIMPLE_FAILURE_MESSAGE);

/**
 * 
 */
USTRUCT(BlueprintType)
struct FRockAbilitySimpleFailureMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer FailureTags;

	UPROPERTY(BlueprintReadWrite)
	FText UserFacingReason;
};
