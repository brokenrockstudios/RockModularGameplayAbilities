// Copyright Broken Rock Studios LLC. All Rights Reserved.

#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "Subsystems/WorldSubsystem.h"

#include "RockGlobalAbilitySystem.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class URockAbilitySystemComponent;
class UObject;
struct FFrame;

USTRUCT()
struct FRockGlobalAppliedAbilityList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<TObjectPtr<URockAbilitySystemComponent>, FGameplayAbilitySpecHandle> Handles;

	void AddToASC(const TSubclassOf<UGameplayAbility>& Ability, URockAbilitySystemComponent* ASC);
	void RemoveFromASC(URockAbilitySystemComponent* ASC);
	void RemoveFromAll();
};

USTRUCT()
struct FRockGlobalAppliedEffectList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<TObjectPtr<URockAbilitySystemComponent>, FActiveGameplayEffectHandle> Handles;

	void AddToASC(const TSubclassOf<UGameplayEffect>& Effect, URockAbilitySystemComponent* ASC);
	void RemoveFromASC(URockAbilitySystemComponent* ASC);
	void RemoveFromAll();
};

/**
 * 
 */
UCLASS()
class ROCKMODULARGAMEPLAYABILITIES_API URockGlobalAbilitySystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	URockGlobalAbilitySystem();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Rock")
	void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Rock")
	void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rock")
	void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rock")
	void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);

	/** Register an ASC with global system and apply any active global effects/abilities. */
	void RegisterASC(URockAbilitySystemComponent* ASC);

	/** Removes an ASC from the global system, along with any active global effects/abilities. */
	void UnregisterASC(URockAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rock")
	TArray<URockAbilitySystemComponent*> GetAllASCs();
	
private:
	UPROPERTY()
	TMap<TSubclassOf<UGameplayAbility>, FRockGlobalAppliedAbilityList> AppliedAbilities;

	UPROPERTY()
	TMap<TSubclassOf<UGameplayEffect>, FRockGlobalAppliedEffectList> AppliedEffects;
protected:
	UPROPERTY()
	TArray<TObjectPtr<URockAbilitySystemComponent>> RegisteredASCs;
};
