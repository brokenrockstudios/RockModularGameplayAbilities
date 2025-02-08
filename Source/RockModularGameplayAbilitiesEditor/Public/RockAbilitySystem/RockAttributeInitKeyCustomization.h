// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

/**
 * Customizes the display and behavior of Rock Attribute Initialization Keys.
 */
class FRockAttributeInitKeyCustomization : public IPropertyTypeCustomization
{
public:
	/** Factory method to create an instance of this customization. */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/**
	 * Customize the header portion of the property.
	 *
	 * @param StructPropertyHandle	The handle to the property being customized.
	 * @param HeaderRow				The row widget that will display the header.
	 * @param StructCustomizationUtils	Utilities to aid in customization.
	 */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	/**
	 * Generates combo box entries for the attribute category.
	 *
	 * @param OutComboBoxStrings	Array to populate with combo box strings.
	 * @param OutToolTips			Array to populate with tool tips.
	 * @param OutRestrictedItems	Array indicating restricted items.
	 * @param bAllowClear			Whether a "clear" option is allowed.
	 * @param bAllowAll				Whether an "all" option is allowed.
	 * @param InItems				Map of available categories.
	 */
	static void GenerateCategoryComboBoxStrings(TArray<TSharedPtr<FString>>& OutComboBoxStrings, TArray<TSharedPtr<SToolTip>>& OutToolTips, TArray<bool>& OutRestrictedItems, bool bAllowClear, bool bAllowAll, TMap<FName, TArray<FName>>* InItems);
	
	/** Returns the current attribute category as a string. */
	FString GenerateCategoryString() const;

	/**
	 * Called when a new attribute category is selected.
	 *
	 * @param SelectedCategory	The newly selected category.
	 */
	void OnCategoryValueSelected(const FString& SelectedCategory);

	/**
	 * Generates combo box entries for the attribute sub-category.
	 *
	 * @param OutComboBoxStrings	Array to populate with combo box strings.
	 * @param OutToolTips			Array to populate with tool tips.
	 * @param OutRestrictedItems	Array indicating restricted items.
	 * @param bAllowClear			Whether a "clear" option is allowed.
	 * @param bAllowAll				Whether an "all" option is allowed.
	 * @param InItems				Map of available sub-categories.
	 * @param PrimaryKey			Property handle for the primary key (category).
	 */
	static void GenerateSubCategoryComboBoxStrings(TArray<TSharedPtr<FString>>& OutComboBoxStrings, TArray<TSharedPtr<SToolTip>>& OutToolTips, TArray<bool>& OutRestrictedItems, bool bAllowClear, bool bAllowAll, TMap<FName, TArray<FName>>* InItems, TSharedPtr<IPropertyHandle> PrimaryKey);
	
	/** Returns the current attribute sub-category as a string. */
	FString GenerateSubCategoryString() const;
	
	/**
	 * Called when a new attribute sub-category is selected.
	 *
	 * @param SelectedSubCategory	The newly selected sub-category.
	 */
	void OnSubCategoryValueSelected(const FString& SelectedSubCategory);

	/**
	 * Customize the children properties of the struct, if necessary.
	 *
	 * @param StructPropertyHandle	The handle to the property being customized.
	 * @param StructBuilder			Builder to add custom children widgets.
	 * @param StructCustomizationUtils	Utilities to aid in customization.
	 */
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:
	/** Map storing attribute categories and their corresponding sub-categories. */
	TMap<FName, TArray<FName>> FNameMap;

	/** Handle to the attribute category property. */
	TSharedPtr<IPropertyHandle> AttributeCategoryPropertyHandle;
	/** Handle to the attribute sub-category property. */
	TSharedPtr<IPropertyHandle> AttributeSubCategoryPropertyHandle;
	/** Utility pointer for property customization operations. */
	TWeakPtr<IPropertyUtilities> PropertyUtilities;
};