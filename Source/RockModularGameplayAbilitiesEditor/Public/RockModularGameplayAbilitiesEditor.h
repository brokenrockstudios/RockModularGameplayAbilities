// Copyright Broken Rock Studios LLC. All Rights Reserved.
// See the LICENSE file for details.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FRockModularGameplayAbilitiesEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    
    void AddRockAttributeInitKeyCustomization();


    static FText Menu_ReloadAttributesText();
    static void Menu_ReloadAttributes();

};
