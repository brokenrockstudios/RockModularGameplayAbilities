// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#include "AbilitySystem/Attributes/RockAttributeSetInitter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"

TSubclassOf<UAttributeSet> CommonFindBestAttributeClass(TArray<TSubclassOf<UAttributeSet>>& ClassList, const FString& PartialName)
{
	for (const TSubclassOf<UAttributeSet> Class : ClassList)
	{
		if (Class->GetName().Contains(PartialName))
		{
			return Class;
		}
	}
	return nullptr;
}

void FRockAttributeSetInitter::PreloadAttributeSetData(const TArray<UCurveTable*>& CurveData)
{
	if (!ensure(CurveData.Num() > 0))
	{
		return;
	}

	/**
	 *	Get list of AttributeSet classes loaded
	 */
	TArray<TSubclassOf<UAttributeSet>> ClassList;
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* TestClass = *ClassIt;
		if (TestClass->IsChildOf(UAttributeSet::StaticClass()))
		{
			ClassList.Add(TestClass);
		}
	}

	/**
	 *	Loop through CurveData table and build sets of Defaults that keyed off of Name + Level
	 */
	for (const UCurveTable* CurTable : CurveData)
	{
		for (const TPair<FName, FRealCurve*>& CurveRow : CurTable->GetRowMap())
		{
			FString RowName = CurveRow.Key.ToString();
			FString ClassName;
			FString SetName;
			FString AttributeName;
			FString Temp;

			//Split the RowName into ClassName (Class) and the put the rest in Temp (Player.MaxHealth)
			RowName.Split(TEXT("."), &Temp, &AttributeName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			//Split the remainder into the SetName (Player) and the AttributeName (MaxHealth)
			Temp.Split(TEXT("."), &ClassName, &SetName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

			//If some of these ended up unpopulated just disregard this row...
			if (!ensure(!ClassName.IsEmpty() && !SetName.IsEmpty() && !AttributeName.IsEmpty()))
			{
				ABILITY_LOG(Error, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Unable to parse row %s in %s"), *RowName, *CurTable->GetName());
				continue;
			}

			// Find the AttributeSet

			TSubclassOf<UAttributeSet> Set = CommonFindBestAttributeClass(ClassList, SetName);
			if (!Set)
			{
				// This is ok, we may have rows in here that don't correspond directly to attributes
				ABILITY_LOG(Error, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Unable to match AttributeSet from %s (row: %s)"), *SetName, *RowName);
				continue;
			}

			// Find the FProperty
			// The IsSupportedProperty() just means "is this a number of a FGameplayAttribute?"
			FProperty* Property = FindFProperty<FProperty>(*Set, *AttributeName);
			if (!IsSupportedProperty(Property))
			{
				ABILITY_LOG(Error, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Unable to match Attribute from %s (row: %s)"), *AttributeName, *RowName);
				continue;
			}

			const FRealCurve* Curve = CurveRow.Value;
			FName ClassFName = FName(*ClassName);
			FAttributeSetDefaultsCollection& DefaultCollection = Defaults.FindOrAdd(ClassFName);

			float FirstLevelFloat = 0.f;
			float LastLevelFloat = 0.f;
			Curve->GetTimeRange(FirstLevelFloat, LastLevelFloat);
			int32 FirstLevel = FMath::RoundToInt32(FirstLevelFloat);
			int32 LastLevel = FMath::RoundToInt32(LastLevelFloat);
			
			// Only log these as warnings, as they're not deal breakers.
			if (FirstLevel != 1)
			{
				ABILITY_LOG(Warning, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData First level should be 1"));
				// continue;
			}
			
			// Check our curve to make sure the keys match the expected format
			int32 ExpectedLevel = 1;
			bool bShouldSkip = false;

			// This whole for loop is just to check if the curve is valid.
			// TODO: We could probably just skip this, once we support interpolation later. 
			for (auto KeyIter = Curve->GetKeyHandleIterator(); KeyIter; ++KeyIter)
			{
				const FKeyHandle& KeyHandle = *KeyIter;
				if (KeyHandle == FKeyHandle::Invalid())
				{
					ABILITY_LOG(Error, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Data contains an invalid key handle (row: %s)"), *RowName);
					bShouldSkip = true;
					break;
				}

				const int32 Level = Curve->GetKeyTimeValuePair(KeyHandle).Key;
				if (ExpectedLevel != Level)
				{
					ABILITY_LOG(Error, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Keys are expected to start at 1 and increase by 1 for every key (row: %s)"), *RowName);
					bShouldSkip = true;
					break;
				}

				++ExpectedLevel;
			}

			if (bShouldSkip)
			{
				continue;
			}

			// GetLastKeyHandle vs MaxTimeRange, we already defined LastLevel above.
			LastLevel = Curve->GetKeyTime(Curve->GetLastKeyHandle());
			DefaultCollection.LevelData.SetNum(FMath::Max(LastLevel, DefaultCollection.LevelData.Num()));
			//At this point we know the Name of this "class"/"group", the AttributeSet, and the Property Name. Now loop through the values on the curve to get the attribute default value at each level.
			// We should be iterating between Level 1 and LastLevel instead of KeyHandleIterator
			for (auto KeyIter = Curve->GetKeyHandleIterator(); KeyIter; ++KeyIter)
			{
				const FKeyHandle& KeyHandle = *KeyIter;

				const TPair<float, float> LevelValuePair = Curve->GetKeyTimeValuePair(KeyHandle);
				const int32 Level = LevelValuePair.Key;
				const float Value = LevelValuePair.Value;

				FAttributeSetDefaults& SetDefaults = DefaultCollection.LevelData[Level - 1];

				FAttributeDefaultValueList* DefaultDataList = SetDefaults.DataMap.Find(Set);
				if (DefaultDataList == nullptr)
				{
					ABILITY_LOG(Verbose, TEXT("Initializing new default set for %s[%d]. PropertySize: %d.. DefaultSize: %d"), *Set->GetName(), Level, Set->GetPropertiesSize(), UAttributeSet::StaticClass()->GetPropertiesSize());

					DefaultDataList = &SetDefaults.DataMap.Add(Set);
				}

				// Import curve value into default data

				check(DefaultDataList);
				DefaultDataList->AddPair(Property, Value);
			}
		}
	}
}

void FRockAttributeSetInitter::InitAttributeSetDefaults(
	UAbilitySystemComponent* AbilitySystemComponent, FName GroupName, int32 Level, bool bInitialInit) const
{
	check(AbilitySystemComponent != nullptr);

	// This whole block will look if the provided group exists in the preloaded data.
	// If it doesn't it checks for the Default group. If that isn't there either, the whole operation is stopped.
	const FAttributeSetDefaultsCollection* Collection = Defaults.Find(GroupName);
	if (!Collection)
	{
		ABILITY_LOG(Error, TEXT("Unable to find DefaultAttributeSet Group %s. Falling back to Defaults"), *GroupName.ToString());
		Collection = Defaults.Find(FName(TEXT("Default")));
		if (!Collection)
		{
			ABILITY_LOG(Error, TEXT("FAttributeSetInitterDiscreteLevels::InitAttributeSetDefaults Default DefaultAttributeSet not found! Skipping Initialization"));
			return;
		}
	}

	if (!Collection->LevelData.IsValidIndex(Level - 1))
	{
		// We could eventually extrapolate values outside the max defined levels
		ABILITY_LOG(Error, TEXT("Init Attribute defaults for Level %d are not defined! Skipping"), Level);
		return;
	}
	
	const FAttributeSetDefaults& SetDefaults = Collection->LevelData[Level - 1];
	// Iterate over all the spawned attribute sets of the provided ASC
	for (const UAttributeSet* Set : AbilitySystemComponent->GetSpawnedAttributes())
	{
		if (!Set)
		{
			continue;
		}
		// Iterate to find the parent classes, as this could be a derived set
		TSubclassOf<UAttributeSet> ParentClass;
		for (ParentClass = Set->GetClass(); ParentClass ? ParentClass->GetSuperClass() : nullptr; ParentClass = ParentClass->GetSuperClass())
		{
			if (SetDefaults.DataMap.Contains(ParentClass))
			{
				ABILITY_LOG(Log, TEXT("Found Parent Class %s"), *ParentClass->GetName());
				break;
			}
		}
		// Check our preloaded data to see if we have any curves for the given attribute set...
		const FAttributeDefaultValueList* DefaultDataList = SetDefaults.DataMap.Find(ParentClass);
		if (DefaultDataList)
		{
			ABILITY_LOG(Log, TEXT("Initializing Set %s"), *Set->GetName());

			for (auto& DataPair : DefaultDataList->List)
			{
				check(DataPair.Property);

				if (Set->ShouldInitProperty(bInitialInit, DataPair.Property))
				{
					FGameplayAttribute AttributeToModify(DataPair.Property);
					AbilitySystemComponent->SetNumericAttributeBase(AttributeToModify, DataPair.Value);
				}
			}
		}
	}

	AbilitySystemComponent->ForceReplication();
}

void FRockAttributeSetInitter::ApplyAttributeDefault(
	UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute& InAttribute, FName GroupName, int32 Level) const
{
	// This is same as AttributeSetInitterDiscreteLevels 
	
	//SCOPE_CYCLE_COUNTER(STAT_InitAttributeSetDefaults);
	const FAttributeSetDefaultsCollection* Collection = Defaults.Find(GroupName);
	if (!Collection)
	{
		ABILITY_LOG(Error, TEXT("Unable to find DefaultAttributeSet Group %s. Falling back to Defaults"), *GroupName.ToString());
		Collection = Defaults.Find(FName(TEXT("Default")));
		if (!Collection)
		{
			ABILITY_LOG(Error, TEXT("FAttributeSetInitterDiscreteLevels::InitAttributeSetDefaults Default DefaultAttributeSet not found! Skipping Initialization"));
			return;
		}
	}

	if (!Collection->LevelData.IsValidIndex(Level - 1))
	{
		// We could eventually extrapolate values outside the max defined levels
		ABILITY_LOG(Error, TEXT("Attribute defaults for Level %d are not defined! Skipping"), Level);
		return;
	}

	const FAttributeSetDefaults& SetDefaults = Collection->LevelData[Level - 1];
	for (const UAttributeSet* Set : AbilitySystemComponent->GetSpawnedAttributes())
	{
		if (!Set)
		{
			continue;
		}

		// Iterate to find the parent classes, as this could be a derived set
		TSubclassOf<UAttributeSet> ParentClass;
		for (ParentClass = Set->GetClass(); ParentClass ? ParentClass->GetSuperClass() : nullptr; ParentClass = ParentClass->GetSuperClass())
		{
			if (SetDefaults.DataMap.Contains(ParentClass))
			{
				break;
			}
		}

		const FAttributeDefaultValueList* DefaultDataList = SetDefaults.DataMap.Find(ParentClass);
		if (DefaultDataList)
		{
			ABILITY_LOG(Log, TEXT("Initializing Set %s"), *Set->GetName());

			for (auto& DataPair : DefaultDataList->List)
			{
				check(DataPair.Property);

				if (DataPair.Property == InAttribute.GetUProperty())
				{
					FGameplayAttribute AttributeToModify(DataPair.Property);
					AbilitySystemComponent->SetNumericAttributeBase(AttributeToModify, DataPair.Value);
				}
			}
		}
	}

	AbilitySystemComponent->ForceReplication();
}

TArray<float> FRockAttributeSetInitter::GetAttributeSetValues(UClass* AttributeSetClass, FProperty* AttributeProperty, FName GroupName) const
{
	TArray<float> AttributeSetValues;
	const FAttributeSetDefaultsCollection* Collection = Defaults.Find(GroupName);
	if (!Collection)
	{
		ABILITY_LOG(Error, TEXT("FAttributeSetInitterDiscreteLevels::InitAttributeSetDefaults Default DefaultAttributeSet not found! Skipping Initialization"));
		return TArray<float>();
	}

	for (const FAttributeSetDefaults& SetDefaults : Collection->LevelData)
	{
		const FAttributeDefaultValueList* DefaultDataList = SetDefaults.DataMap.Find(AttributeSetClass);
		if (DefaultDataList)
		{
			for (auto& DataPair : DefaultDataList->List)
			{
				check(DataPair.Property);
				if (DataPair.Property == AttributeProperty)
				{
					AttributeSetValues.Add(DataPair.Value);
				}
			}
		}
	}
	return AttributeSetValues;
}

bool FRockAttributeSetInitter::IsSupportedProperty(FProperty* Property) const
{
	return (Property && (CastField<FNumericProperty>(Property) || FGameplayAttribute::IsGameplayAttributeDataProperty(Property)));
}
