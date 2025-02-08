// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#include "RockModularGameplayAbilitiesEditor.h"

#include "AbilitySystem/Global/RockAbilitySystemGlobals.h"
#include "RockAbilitySystem/RockAttributeInitKeyCustomization.h"


#define LOCTEXT_NAMESPACE "FRockModularGameplayAbilitiesEditorModule"

void FRockModularGameplayAbilitiesEditorModule::StartupModule()
{
	AddRockAttributeInitKeyCustomization();
}

void FRockModularGameplayAbilitiesEditorModule::ShutdownModule()
{
    
}

void FRockModularGameplayAbilitiesEditorModule::AddRockAttributeInitKeyCustomization()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout("RockAttributeInitializationKey", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FRockAttributeInitKeyCustomization::MakeInstance));

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
		FToolMenuSection& Section = Menu->AddSection("RockGameplayAbilitiesCore", LOCTEXT("RockGASUtilities", "RockGASUtilities"));
		Section.AddEntry(FToolMenuEntry::InitMenuEntry(
			"ReloadAttributeTables",
			TAttribute<FText>::Create(&FRockModularGameplayAbilitiesEditorModule::Menu_ReloadAttributesText),
			LOCTEXT("ReloadAttributeTables", "Reload Attribute Tables"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "DeveloperTools.MenuIcon"),
			FUIAction(FExecuteAction::CreateStatic(&FRockModularGameplayAbilitiesEditorModule::Menu_ReloadAttributes))
		));
	}
}


FText FRockModularGameplayAbilitiesEditorModule::Menu_ReloadAttributesText()
{
	return LOCTEXT("ReloadAttributeTables", "Reload Attribute Tables");
}
void FRockModularGameplayAbilitiesEditorModule::Menu_ReloadAttributes()
{
	URockAbilitySystemGlobals::Get().ReloadAttributeDefaults();
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRockModularGameplayAbilitiesEditorModule, RockModularGameplayAbilitiesEditor)