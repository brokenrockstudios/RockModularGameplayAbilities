// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#include "Animation/AnimNotify/RockAnimNotifyState_ApplyLooseGameplayTag.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

URockAnimNotifyState_ApplyLooseGameplayTag::URockAnimNotifyState_ApplyLooseGameplayTag(const FObjectInitializer& ObjectInitializer)
{
	bIsNativeBranchingPoint = true;
}

void URockAnimNotifyState_ApplyLooseGameplayTag::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);

	if (BranchingPointPayload.SkelMeshComponent == nullptr)
	{
		return;
	}
	const AActor * actor = BranchingPointPayload.SkelMeshComponent->GetOwner();
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(actor))
	{
		ASC->AddLooseGameplayTags(TagsToApply);
	}
	
}

void URockAnimNotifyState_ApplyLooseGameplayTag::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (BranchingPointPayload.SkelMeshComponent == nullptr)
	{
		return;
	}
	const AActor * actor = BranchingPointPayload.SkelMeshComponent->GetOwner();
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(actor))
	{
		ASC->RemoveLooseGameplayTags(TagsToApply);
	}
	Super::BranchingPointNotifyEnd(BranchingPointPayload);
}

#if WITH_EDITOR
bool URockAnimNotifyState_ApplyLooseGameplayTag::CanBePlaced(UAnimSequenceBase* Animation) const
{
	//UAnimMontage : UAnimCompositeBase : UAnimSequenceBase
	return Animation && Animation->IsA<UAnimMontage>();
}
#endif

FString URockAnimNotifyState_ApplyLooseGameplayTag::GetNotifyName_Implementation() const
{
	FString NotifyName;
#if WITH_EDITORONLY_DATA
	if (const UObject* ClassGeneratedBy = GetClass()->ClassGeneratedBy)
	{
		// For blueprint types, use the generated class's name for a cleaner display.
		NotifyName = ClassGeneratedBy->GetName();
	}
	else
#endif
	{
		// For native classes, the class name is already clean.
		NotifyName = GetClass()->GetName();
	}
	// Remove the default prefix.
	NotifyName.ReplaceInline(TEXT("AnimNotifyState_"), TEXT(""), ESearchCase::CaseSensitive);
	
	// Merge the notify name with the simple string representation of the tags.
	FString MergedName = FString::Printf(TEXT("%s_%s"), *NotifyName, *TagsToApply.ToStringSimple());
	return MergedName;
}
