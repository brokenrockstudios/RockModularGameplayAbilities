// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#include "AbilitySystem/Components/RockAbilitySystemComponent.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystem/Assets/RockAbilityTagRelationshipMapping.h"
#include "AbilitySystem/Global/RockGlobalAbilitySystem.h"
#include "Animation/RockAnimInstance.h"
#include "Logging/RockLogging.h"

URockAbilitySystemComponent::URockAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
	
	FMemory::Memset(ActivationGroupCounts, 0, sizeof(ActivationGroupCounts));
}

void URockAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (URockGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<URockGlobalAbilitySystem>(GetWorld()))
	{
		GlobalAbilitySystem->UnregisterASC(this);
	}

	Super::EndPlay(EndPlayReason);
}

URockAbilitySystemComponent* URockAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor, bool bSearchComponents)
{
	if (!IsValid(Actor))
	{
		return nullptr;
	}

	if (const IAbilitySystemInterface* InventoryInterface = Cast<IAbilitySystemInterface>(Actor))
	{
		return Cast<URockAbilitySystemComponent>(InventoryInterface->GetAbilitySystemComponent());
	}

	if (bSearchComponents)
	{
		if (URockAbilitySystemComponent* IC = Actor->GetComponentByClass<URockAbilitySystemComponent>())
		{
			return IC;
		}
	}

	return nullptr;
}

void URockAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);

	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (bHasNewPawnAvatar)
	{
		// Notify all abilities that a new pawn avatar has been set
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			PRAGMA_DISABLE_DEPRECATION_WARNINGS
			ensureMsgf(AbilitySpec.Ability && AbilitySpec.Ability->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced, TEXT("InitAbilityActorInfo: All Abilities should be Instanced (NonInstanced is being deprecated due to usability issues)."));
			PRAGMA_ENABLE_DEPRECATION_WARNINGS

			const URockGameplayAbility* RockAbilityCDO = Cast<URockGameplayAbility>(AbilitySpec.Ability);
			if (!RockAbilityCDO)
			{
				continue;
			}
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			for (UGameplayAbility* AbilityInstance : Instances)
			{
				URockGameplayAbility* RockAbilityInstance = Cast<URockGameplayAbility>(AbilityInstance);
				if (RockAbilityInstance)
				{
					// Ability instances may be missing for replays
					RockAbilityInstance->NativeOnPawnAvatarSet();
				}
			}
		}

		// Register with the global system once we actually have a pawn avatar. We wait until this time since some globally-applied effects may require an avatar.
		if (URockGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<URockGlobalAbilitySystem>(GetWorld()))
		{
			GlobalAbilitySystem->RegisterASC(this);
		}

		if (URockAnimInstance* RockAnimInst = Cast<URockAnimInstance>(ActorInfo->GetAnimInstance()))
		{
			RockAnimInst->InitializeWithAbilitySystem(this);
		}

		TryActivateAbilitiesOnSpawn();
	}
}

void URockAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (const URockGameplayAbility* RockAbilityCDO = Cast<URockGameplayAbility>(AbilitySpec.Ability))
		{
			RockAbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
		}
	}
}


void URockAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility)
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.IsActive())
		{
			continue;
		}

		const URockGameplayAbility* RockAbilityCDO = Cast<URockGameplayAbility>(AbilitySpec.Ability);
		if (!RockAbilityCDO)
		{
			UE_LOG(LogRockAbilitySystem,
				Error,
				TEXT("CancelAbilitiesByFunc: Non-RockGameplayAbility %s was Granted to ASC. Skipping."),
				*AbilitySpec.Ability.GetName());
			continue;
		}

		PRAGMA_DISABLE_DEPRECATION_WARNINGS
		ensureMsgf(AbilitySpec.Ability->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced, TEXT("CancelAbilitiesByFunc: All Abilities should be Instanced (NonInstanced is being deprecated due to usability issues)."));
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
		
		// Cancel all the spawned instances, not the CDO.
		TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
		for (UGameplayAbility* AbilityInstance : Instances)
		{
			URockGameplayAbility* RockAbilityInstance = CastChecked<URockGameplayAbility>(AbilityInstance);

			if (ShouldCancelFunc(RockAbilityInstance, AbilitySpec.Handle))
			{
				if (RockAbilityInstance->CanBeCanceled())
				{
					RockAbilityInstance->CancelAbility(AbilitySpec.Handle,
						AbilityActorInfo.Get(),
						RockAbilityInstance->GetCurrentActivationInfo(),
						bReplicateCancelAbility);
				}
				else
				{
					UE_LOG(LogRockAbilitySystem,
						Error,
						TEXT("CancelAbilitiesByFunc: Can't cancel ability [%s] because CanBeCanceled is false."),
						*RockAbilityInstance->GetName());
				}
			}
		}
	}
}

bool URockAbilitySystemComponent::IsActivationGroupBlocked(ERockAbilityActivationGroup Group) const
{
		bool bBlocked = false;

		switch (Group)
		{
		case ERockAbilityActivationGroup::Independent:
		{
				// Independent abilities are never blocked.
				bBlocked = false;
				break;
		}
		case ERockAbilityActivationGroup::Exclusive_Replaceable:
		case ERockAbilityActivationGroup::Exclusive_Blocking:
		{
				
			// Exclusive abilities can activate if nothing is blocking.
			bBlocked = (ActivationGroupCounts[static_cast<uint8>(ERockAbilityActivationGroup::Exclusive_Blocking)] > 0);
			break;
		}
		default:
		{
				checkf(false, TEXT("IsActivationGroupBlocked: Invalid ActivationGroup [%d]\n"), static_cast<uint8>(Group));
				break;
		}
		}

		return bBlocked;
}

void URockAbilitySystemComponent::AddAbilityToActivationGroup(ERockAbilityActivationGroup Group, URockGameplayAbility* RockAbility)
{
	check(RockAbility);
	check(ActivationGroupCounts[static_cast<uint8>(Group)] < INT32_MAX);

	ActivationGroupCounts[static_cast<uint8>(Group)]++;

	const bool bReplicateCancelAbility = false;

	switch (Group)
	{
	case ERockAbilityActivationGroup::Independent:
		// Independent abilities do not cancel any other abilities.
			break;

	case ERockAbilityActivationGroup::Exclusive_Replaceable:
	case ERockAbilityActivationGroup::Exclusive_Blocking:
		CancelActivationGroupAbilities(ERockAbilityActivationGroup::Exclusive_Replaceable, RockAbility, bReplicateCancelAbility);
		break;

	default:
		checkf(false, TEXT("AddAbilityToActivationGroup: Invalid ActivationGroup [%d]\n"), static_cast<uint8>(Group));
		break;
	}

	const int32 ExclusiveCount = ActivationGroupCounts[static_cast<uint8>(ERockAbilityActivationGroup::Exclusive_Replaceable)]
		+ ActivationGroupCounts[static_cast<uint8>(ERockAbilityActivationGroup::Exclusive_Blocking)];
	if (!ensure(ExclusiveCount <= 1))
	{
		UE_LOG(LogRockAbilitySystem, Error, TEXT("AddAbilityToActivationGroup: Multiple exclusive abilities are running."));
	}
}

void URockAbilitySystemComponent::RemoveAbilityFromActivationGroup(ERockAbilityActivationGroup Group, URockGameplayAbility* RockAbility)
{
	check(RockAbility);
	check(ActivationGroupCounts[static_cast<uint8>(Group)] > 0);

	ActivationGroupCounts[static_cast<uint8>(Group)]--;
}

void URockAbilitySystemComponent::CancelActivationGroupAbilities(
	ERockAbilityActivationGroup Group, URockGameplayAbility* IgnoreRockAbility, bool bReplicateCancelAbility)
{
	auto ShouldCancelFunc = [this, Group, IgnoreRockAbility](const URockGameplayAbility* RockAbility, FGameplayAbilitySpecHandle Handle)
	{
		return ((RockAbility->GetActivationGroup() == Group) && (RockAbility != IgnoreRockAbility));
	};

	CancelAbilitiesByFunc(ShouldCancelFunc, bReplicateCancelAbility);
}

void URockAbilitySystemComponent::DeferredSetBaseAttributeValueFromReplication(
	const FGameplayAttribute& attribute, const FGameplayAttributeData& newValue)
{
	const float OldValue = ActiveGameplayEffects.GetAttributeBaseValue(attribute);
	ActiveGameplayEffects.SetAttributeBaseValue(attribute, newValue.GetBaseValue());
	SetBaseAttributeValueFromReplication(attribute, newValue.GetBaseValue(), OldValue);
	// TODO: You can process deferred delegates here
}

void URockAbilitySystemComponent::DeferredSetBaseAttributeValueFromReplication(
	const FGameplayAttribute& attribute, float newValue)
{
	const float OldValue = ActiveGameplayEffects.GetAttributeBaseValue(attribute);
	ActiveGameplayEffects.SetAttributeBaseValue(attribute, newValue);
	SetBaseAttributeValueFromReplication(attribute, newValue, OldValue);
	// TODO: You can process deferred delegates here
}

void URockAbilitySystemComponent::GetAbilityTargetData(
	const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo,
	FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
	const TSharedPtr<FAbilityReplicatedDataCache> ReplicatedData = AbilityTargetDataMap.Find(
	FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, ActivationInfo.GetActivationPredictionKey()));
	if (ReplicatedData.IsValid())
	{
		OutTargetDataHandle = ReplicatedData->TargetData;
	}
}

void URockAbilitySystemComponent::SetTagRelationshipMapping(URockAbilityTagRelationshipMapping* NewMapping)
{
	TagRelationshipMapping = NewMapping;
}

void URockAbilitySystemComponent::GetAdditionalActivationTagRequirements(
	const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, &OutActivationRequired, &OutActivationBlocked);
	}
}

void URockAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);

	if (URockGameplayAbility* RockAbility = Cast<URockGameplayAbility>(Ability))
	{
		AddAbilityToActivationGroup(RockAbility->GetActivationGroup(), RockAbility);
	}
}

void URockAbilitySystemComponent::NotifyAbilityFailed(
	const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);

	if (APawn* Avatar = Cast<APawn>(GetAvatarActor()))
	{
		if (!Avatar->IsLocallyControlled() && Ability->IsSupportedForNetworking())
		{
			ClientNotifyAbilityFailed(Ability, FailureReason);
			return;
		}
	}

	HandleAbilityFailed(Ability, FailureReason);
}

void URockAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

	if (URockGameplayAbility* RockAbility = Cast<URockGameplayAbility>(Ability))
	{
		RemoveAbilityFromActivationGroup(RockAbility->GetActivationGroup(), RockAbility);
	}
}

void URockAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(
	const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags,
	bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	FGameplayTagContainer ModifiedBlockTags = BlockTags;
	FGameplayTagContainer ModifiedCancelTags = CancelTags;

	if (TagRelationshipMapping)
	{
		// Use the mapping to expand the ability tags into block and cancel tag
		TagRelationshipMapping->GetAbilityTagsToBlockAndCancel(AbilityTags, &ModifiedBlockTags, &ModifiedCancelTags);
	}

	Super::ApplyAbilityBlockAndCancelTags(AbilityTags,
		RequestingAbility,
		bEnableBlockTags,
		ModifiedBlockTags,
		bExecuteCancelTags,
		ModifiedCancelTags);

	//@TODO: Apply any special logic like blocking input or movement
}

void URockAbilitySystemComponent::HandleChangeAbilityCanBeCanceled(
	const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled)
{
	Super::HandleChangeAbilityCanBeCanceled(AbilityTags, RequestingAbility, bCanBeCanceled);

	//@TODO: Apply any special logic like blocking input or movement
}

void URockAbilitySystemComponent::RemoveGameplayCue_Internal(const FGameplayTag GameplayCueTag, FActiveGameplayCueContainer& GameplayCueContainer)
{
	//Super::RemoveGameplayCue_Internal(GameplayCueTag, GameplayCueContainer);
	// TODO: This whole bit of code is likely fixed now properly in ASC in UE 5.4 and can possibly be removed in 5.4
	if (IsOwnerActorAuthoritative())
	{
		int32 NumMatchingCues = 0;
		for (const FActiveGameplayCue& GameplayCue : GameplayCueContainer.GameplayCues)
		{
			NumMatchingCues += (GameplayCue.GameplayCueTag == GameplayCueTag);
		}

		if (NumMatchingCues > 0)
		{
			// AbilitySystem.GameplayCueNotifyTagCheckOnRemove assumes the tag is removed before any invocation of EGameplayCueEvent::Removed.
			// We cannot use GameplayCueContainer.RemoveCue because that removes the cues while updating the TagMap.
			// Instead, we need to manually count the removals, update the tag map, then Invoke the Cue events while removing the Cues.
			UpdateTagMap(GameplayCueTag, -NumMatchingCues);

			for (int32 Index = GameplayCueContainer.GameplayCues.Num() - 1; Index >= 0; --Index)
			{
				const FActiveGameplayCue& GameplayCue = GameplayCueContainer.GameplayCues[Index];
				if (GameplayCue.GameplayCueTag == GameplayCueTag)
				{
					// Call on server here, clients get it from repnotify on the GameplayCueContainer rather than a multicast rpc
					InvokeGameplayCueEvent(GameplayCueTag, EGameplayCueEvent::Removed, GameplayCue.Parameters);
					GameplayCueContainer.GameplayCues.RemoveAt(Index);
				}
			}

			// Ensure that the clients are aware of these changes ASAP
			GameplayCueContainer.MarkArrayDirty();
			ForceReplication();
		}
	}
	else if (ScopedPredictionKey.IsLocalClientKey())
	{
		GameplayCueContainer.PredictiveRemove(GameplayCueTag);
	}
}

void URockAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation(
	const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	HandleAbilityFailed(Ability, FailureReason);
}

void URockAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	//UE_LOG(LogRockAbilitySystem, Warning, TEXT("Ability %s failed to activate (tags: %s)"), *GetPathNameSafe(Ability), *FailureReason.ToString());

	if (const URockGameplayAbility* RockAbility = Cast<const URockGameplayAbility>(Ability))
	{
		RockAbility->OnAbilityFailedToActivate(FailureReason);
	}
}

