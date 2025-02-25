// Copyright Broken Rock Studios LLC. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "RockGameplayTagFloatContainer.generated.h"

struct FRockGameplayTagFloatContainer;
struct FNetDeltaSerializeInfo;

/**
 * Represents one float of a gameplay tag (tag + count)
 */
USTRUCT(BlueprintType)
struct FRockGameplayTagFloat : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FRockGameplayTagFloat()
	{}

	FRockGameplayTagFloat(FGameplayTag InTag, float InFloatValue)
		: Tag(InTag)
		, FloatValue(InFloatValue)
	{
	}

	FString GetDebugString() const;

private:
	friend FRockGameplayTagFloatContainer;

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	float FloatValue = 0;
};

/** Container of gameplay tag stacks */
USTRUCT(BlueprintType)
struct FRockGameplayTagFloatContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	FRockGameplayTagFloatContainer()
	//	: Owner(nullptr)
	{
	}

public:
	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	void AddFloat(FGameplayTag Tag, float FloatValue);

	// Sets the stack count of the specified tag (removing the tag if StackCount is below 1)
	void SetFloat(FGameplayTag Tag, float FloatValue);
	
	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 0)
	void SubtractFloat(FGameplayTag Tag, float FloatValue);

	// Remove the entry for the specified tag
	void RemoveTag(FGameplayTag Tag);
    	
	
	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	float GetFloatValue(FGameplayTag Tag) const
	{
		return TagToFloatMap.FindRef(Tag);
	}

	// Returns true if there is at least one stack of the specified tag
	bool ContainsTag(FGameplayTag Tag) const
	{
		return TagToFloatMap.Contains(Tag);
	}

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FRockGameplayTagFloat, FRockGameplayTagFloatContainer>(Stacks, DeltaParms, *this);
	}

private:
	// Replicated list of gameplay tag stacks
	UPROPERTY()
	TArray<FRockGameplayTagFloat> Stacks;
	
	// Accelerated list of tag stacks for queries
	TMap<FGameplayTag, float> TagToFloatMap;
};

template<>
struct TStructOpsTypeTraits<FRockGameplayTagFloatContainer> : public TStructOpsTypeTraitsBase2<FRockGameplayTagFloatContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
