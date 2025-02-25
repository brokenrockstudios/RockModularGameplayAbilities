// Copyright Broken Rock Studios LLC. All Rights Reserved.

#include "AbilitySystem/Containers/RockGameplayTagFloatContainer.h"

FString FRockGameplayTagFloat::GetDebugString() const
{
	return FString::Printf(TEXT("%sx%f"), *Tag.ToString(), FloatValue);
}

//////////////////////////////////////////////////////////////////////
// FGameplayTagStackContainer

void FRockGameplayTagFloatContainer::AddFloat(FGameplayTag Tag, float FloatValue)
{
	if (!Tag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to AddStack"), ELogVerbosity::Warning);
		return;
	}

	if (FloatValue > 0)
	{
		for (FRockGameplayTagFloat& Stack : Stacks)
		{
			if (Stack.Tag == Tag)
			{
				const float NewCount = Stack.FloatValue+ FloatValue;
				Stack.FloatValue = NewCount;
				TagToFloatMap[Tag] = NewCount;
				MarkItemDirty(Stack);
				return;
			}
		}

		FRockGameplayTagFloat& NewStack = Stacks.Emplace_GetRef(Tag, FloatValue);
		MarkItemDirty(NewStack);
		TagToFloatMap.Add(Tag, FloatValue);
	}
}

void FRockGameplayTagFloatContainer::SetFloat(FGameplayTag Tag, float FloatValue)
{
	if (!Tag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to SetStack"), ELogVerbosity::Warning);
		return;
	}
	
	if (FloatValue > 0)
	{
		for (FRockGameplayTagFloat& Stack : Stacks)
		{
			if (Stack.Tag == Tag)
			{
				const float NewCount = FloatValue;
				Stack.FloatValue = NewCount;
				TagToFloatMap[Tag] = NewCount;
				MarkItemDirty(Stack);
				return;
			}
		}

		FRockGameplayTagFloat& NewStack = Stacks.Emplace_GetRef(Tag, FloatValue);
		MarkItemDirty(NewStack);
		TagToFloatMap.Add(Tag, FloatValue);
	}
}

void FRockGameplayTagFloatContainer::SubtractFloat(FGameplayTag Tag, float FloatValue)
{
	if (!Tag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to RemoveStack"), ELogVerbosity::Warning);
		return;
	}

	//@TODO: Should we error if you try to remove a stack that doesn't exist or has a smaller count?
	if (FloatValue > 0)
	{
		for (auto It = Stacks.CreateIterator(); It; ++It)
		{
			FRockGameplayTagFloat& Stack = *It;
			if (Stack.Tag == Tag)
			{
				if (Stack.FloatValue <= FloatValue)
				{
					It.RemoveCurrent();
					TagToFloatMap.Remove(Tag);
					MarkArrayDirty();
				}
				else
				{
					const float NewCount = Stack.FloatValue - FloatValue;
					Stack.FloatValue = NewCount;
					TagToFloatMap[Tag] = NewCount;
					MarkItemDirty(Stack);
				}
				return;
			}
		}
	}
}

void FRockGameplayTagFloatContainer::RemoveTag(FGameplayTag Tag)
{
	if (!Tag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to RemoveTag"), ELogVerbosity::Warning);
		return;
	}

	for (auto It = Stacks.CreateIterator(); It; ++It)
	{
		FRockGameplayTagFloat& Stack = *It;
		if (Stack.Tag == Tag)
		{
			It.RemoveCurrent();
			TagToFloatMap.Remove(Tag);
			MarkArrayDirty();
			return;
		}
	}
}

void FRockGameplayTagFloatContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FGameplayTag Tag = Stacks[Index].Tag;
		TagToFloatMap.Remove(Tag);
	}
}

void FRockGameplayTagFloatContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FRockGameplayTagFloat& Stack = Stacks[Index];
		TagToFloatMap.Add(Stack.Tag, Stack.FloatValue);
	}
}

void FRockGameplayTagFloatContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FRockGameplayTagFloat& Stack = Stacks[Index];
		// Original: TagToCountMap[Stack.Tag] = Stack.StackCount;
		// Suggested fix. 
		TagToFloatMap.Emplace(Stack.Tag, Stack.FloatValue);		
		
	}
}
