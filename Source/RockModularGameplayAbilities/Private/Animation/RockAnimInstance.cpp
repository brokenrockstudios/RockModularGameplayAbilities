// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.


#include "Animation/RockAnimInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include "AbilitySystemGlobals.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RockAnimInstance)


URockAnimInstance::URockAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URockAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	GameplayTagPropertyMap.Initialize(this, ASC);
}

#if WITH_EDITOR
EDataValidationResult URockAnimInstance::IsDataValid(FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);

	GameplayTagPropertyMap.IsDataValid(this, Context);

	return ((Context.GetNumErrors() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif // WITH_EDITOR

void URockAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (const AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void URockAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Can this be refactored to be more based around Movement
	// Such that would support both Character and Mover 2.0?
	// An interface or something else?
	// Or should this be implemented in the 'game' and not in the RockModularGameplayAbilities plugin?
	
	// const ALyraCharacter* Character = Cast<ALyraCharacter>(GetOwningActor());
	// if (!Character)
	// {
	// 	return;
	// }
	//
	// ULyraCharacterMovementComponent* CharMoveComp = CastChecked<ULyraCharacterMovementComponent>(Character->GetCharacterMovement());
	// const FLyraCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();
	// GroundDistance = GroundInfo.GroundDistance;
}

