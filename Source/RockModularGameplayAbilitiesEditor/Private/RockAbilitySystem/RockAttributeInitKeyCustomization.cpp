// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#include "RockAbilitySystem/RockAttributeInitKeyCustomization.h"

#include "DetailWidgetRow.h"
#include "PropertyCustomizationHelpers.h"
#include "AbilitySystem/Attributes/RockAttributeSet.h"
#include "AbilitySystem/Global/RockAbilitySystemGlobals.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RockAttributeInitKeyCustomization)

#define LOCTEXT_NAMESPACE "RockAttributeBasedFloatDetails"

TSharedRef<IPropertyTypeCustomization> FRockAttributeInitKeyCustomization::MakeInstance()
{
	return MakeShareable(new FRockAttributeInitKeyCustomization());
}

/**
 * Helper function to parse a row name into its primary and secondary parts.
 *
 * Expected row format (from the end):
 *   - First split: "A.B.C.D" => Temp="A.B.C", AttributeName="D"
 *   - Second split: Temp ("A.B.C") => ClassName="A.B", SetName="C"
 *   - Third split: ClassName ("A.B") => OutPrimary="A", OutSecondary="B"
 *
 * @param RowName		The full row name string.
 * @param OutPrimary	(Output) The primary token.
 * @param OutSecondary	(Output) The secondary token.
 * @return true if the extraction was successful.
 */
bool ExtractPrimaryAndSecondary(const FString& RowName, FString& OutPrimary, FString& OutSecondary)
{
	FString Temp, ClassName, SetName, AttributeName;
	// Split from the end: get "Temp" and "AttributeName"
	if (!RowName.Split(TEXT("."), &Temp, &AttributeName, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
	{
		return false;
	}
	// Split "Temp" into "ClassName" and "SetName"
	if (!Temp.Split(TEXT("."), &ClassName, &SetName, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
	{
		return false;
	}
	// Finally, split "ClassName" into primary and secondary tokens
	return ClassName.Split(TEXT("."), &OutPrimary, &OutSecondary, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
}

void FRockAttributeInitKeyCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	const URockAbilitySystemGlobals& Globals = URockAbilitySystemGlobals::Get();
	TArray<UCurveTable*> LoadedCurves;
	for (const auto& Curve : Globals.GetCurveInitTables())
	{
		UCurveTable* AttributeTable = Cast<UCurveTable>(Curve.TryLoad());
		LoadedCurves.Add(AttributeTable);
	}

	FNameMap.Reset();
	for (const UCurveTable* CurTable : LoadedCurves)
	{
		if (!IsValid(CurTable))
		{
			continue;
		}

		for (const TPair<FName, FRealCurve*>& CurveRow : CurTable->GetRowMap())
		{
			FString RowName = CurveRow.Key.ToString();
			FString Primary;
			FString Secondary;
			if(ExtractPrimaryAndSecondary(RowName, Primary, Secondary))
			{
				TArray<FName>& Rows = FNameMap.FindOrAdd(FName(*Primary));
				Rows.AddUnique(FName(*Secondary));
			}
		}
	}

	PropertyUtilities = StructCustomizationUtils.GetPropertyUtilities();

	AttributeCategoryPropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRockAttributeInitializationKey, AttributeInitCategory));
	AttributeSubCategoryPropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRockAttributeInitializationKey, AttributeInitSubCategory));
	if (AttributeCategoryPropertyHandle.IsValid() && AttributeSubCategoryPropertyHandle.IsValid())
	{
		HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(600)
		.MaxDesiredWidth(4096)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				  .HAlign(HAlign_Fill)
				  .Padding(1.f, 0.f)
				[
					PropertyCustomizationHelpers::MakePropertyComboBox(
						AttributeCategoryPropertyHandle,
                        FOnGetPropertyComboBoxStrings::CreateStatic(&FRockAttributeInitKeyCustomization::GenerateCategoryComboBoxStrings, true, false, &FNameMap),
                        FOnGetPropertyComboBoxValue::CreateSP(this, &FRockAttributeInitKeyCustomization::GenerateCategoryString),
                        FOnPropertyComboBoxValueSelected::CreateSP(this, &FRockAttributeInitKeyCustomization::OnCategoryValueSelected))
				]
				+ SVerticalBox::Slot()
				  .HAlign(HAlign_Fill)
				  .Padding(2.f, 0.f)
				[
					PropertyCustomizationHelpers::MakePropertyComboBox(
						AttributeCategoryPropertyHandle,
                        FOnGetPropertyComboBoxStrings::CreateStatic(&FRockAttributeInitKeyCustomization::GenerateSubCategoryComboBoxStrings, true, false, &FNameMap, AttributeCategoryPropertyHandle),
                        FOnGetPropertyComboBoxValue::CreateSP(this, &FRockAttributeInitKeyCustomization::GenerateSubCategoryString),
                        FOnPropertyComboBoxValueSelected::CreateSP(this, &FRockAttributeInitKeyCustomization::OnSubCategoryValueSelected))
				]
			]
		];
	}
}


void FRockAttributeInitKeyCustomization::GenerateCategoryComboBoxStrings(TArray<TSharedPtr<FString>>& OutComboBoxStrings, TArray<TSharedPtr<SToolTip>>& OutToolTips, TArray<bool>& OutRestrictedItems, bool bAllowClear, bool bAllowAll,
                                                                    TMap<FName, TArray<FName>>* InItems)
{
	if (InItems == nullptr)
	{
		return;
	}
	for (auto Iter = InItems->CreateConstIterator(); Iter; ++Iter)
	{
		OutComboBoxStrings.Add(MakeShared<FString>(Iter.Key().ToString()));
	}
}

void FRockAttributeInitKeyCustomization::GenerateSubCategoryComboBoxStrings(TArray<TSharedPtr<FString>>& OutComboBoxStrings, TArray<TSharedPtr<SToolTip>>& OutToolTips, TArray<bool>& OutRestrictedItems, bool bAllowClear, bool bAllowAll,
                                                                      TMap<FName, TArray<FName>>* InItems, TSharedPtr<IPropertyHandle> PrimaryKey)
{
	void* TagDataPtr = nullptr;
	PrimaryKey->GetValueData(TagDataPtr);
	const FName* TagPtr = static_cast<FName*>(TagDataPtr);

	if (TagPtr == nullptr || TagPtr->IsNone())
	{
		OutComboBoxStrings.Add(MakeShared<FString>("Invalid"));
	}
	else
	{
		const auto Arr = InItems->Find(*TagPtr);
		if (Arr)
		{
			for (auto& Name : *Arr)
			{
				OutComboBoxStrings.Add(MakeShared<FString>(Name.ToString()));
			}
		}
	}
}

FString FRockAttributeInitKeyCustomization::GenerateCategoryString() const
{
	void* TagDataPtr = nullptr;
	AttributeCategoryPropertyHandle->GetValueData(TagDataPtr);
	const FName* TagPtr = static_cast<FName*>(TagDataPtr);
	FString TagString = TagPtr ? TagPtr->ToString() : "Invalid";
	return TagString;
}

FString FRockAttributeInitKeyCustomization::GenerateSubCategoryString() const
{
	void* TagDataPtr = nullptr;
	AttributeSubCategoryPropertyHandle->GetValueData(TagDataPtr);
	const FName* TagPtr = static_cast<FName*>(TagDataPtr);
	FString TagString = TagPtr ? TagPtr->ToString() : "Invalid";
	return TagString;
}

void FRockAttributeInitKeyCustomization::OnCategoryValueSelected(const FString& SelectedCategory)
{
	if (AttributeCategoryPropertyHandle.IsValid())
	{
	    AttributeCategoryPropertyHandle->SetValue(FName(*SelectedCategory));
	}
	if (AttributeSubCategoryPropertyHandle.IsValid()) 
	{
	    AttributeSubCategoryPropertyHandle->ResetToDefault();
	}
}

void FRockAttributeInitKeyCustomization::OnSubCategoryValueSelected(const FString& SelectedSubCategory)
{
	if (AttributeSubCategoryPropertyHandle.IsValid())
	{
		AttributeSubCategoryPropertyHandle->SetValue(FName(*SelectedSubCategory));
	}
}

void FRockAttributeInitKeyCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// No child properties to customize.
}

#undef LOCTEXT_NAMESPACE