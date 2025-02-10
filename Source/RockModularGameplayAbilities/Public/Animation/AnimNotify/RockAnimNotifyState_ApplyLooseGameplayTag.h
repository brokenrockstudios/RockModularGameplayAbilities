// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "RockAnimNotifyState_ApplyLooseGameplayTag.generated.h"

/**
 * Anim notify state that applies and removes loose gameplay tags for the duration of the anim notify state.
 */
UCLASS()
class ROCKMODULARGAMEPLAYABILITIES_API URockAnimNotifyState_ApplyLooseGameplayTag : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	URockAnimNotifyState_ApplyLooseGameplayTag( const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get() );

	// ~Begin UAnimNotifyState
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	// ~End UAnimNotifyState

#if WITH_EDITOR
	virtual bool CanBePlaced(UAnimSequenceBase* Animation) const override;
#endif

	virtual FString GetNotifyName_Implementation() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rock|GameplayTags")
	FGameplayTagContainer TagsToApply;
};
