// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#include "AbilitySystem/Components/RockAbilitySystemComponent.h"


// Sets default values for this component's properties
URockAbilitySystemComponent::URockAbilitySystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


URockAbilitySystemComponent::URockAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
	
	FMemory::Memset(ActivationGroupCounts, 0, sizeof(ActivationGroupCounts));
}

// Called when the game starts
void URockAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void URockAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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

