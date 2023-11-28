#pragma once

#include "CoreMinimal.h"
#include "CogWindow.h"
#include "CogWindowConfig.h"
#include "CogWindow_Settings.generated.h"

class UCogEngineConfig_Settings;

//--------------------------------------------------------------------------------------------------------------------------
class COGWINDOW_API FCogWindow_Settings : public FCogWindow
{
    typedef FCogWindow Super;

public:
    
    virtual void Initialize() override;

    virtual void RenderTick(float DeltaTime) override;

    const UCogWindowConfig_Settings* GetSettingsConfig() const { return Config; }

    void SetDPIScale(float Value);

protected: 

    virtual void RenderContent() override;

    virtual void PreSaveConfig() override;

    virtual void ResetConfig() override;

    TObjectPtr<UCogWindowConfig_Settings> Config = nullptr;
};


//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Config = Cog)
class UCogWindowConfig_Settings : public UCogWindowConfig
{
    GENERATED_BODY()

public:

    UPROPERTY(Config)
    float DPIScale = 1.0f;

    UPROPERTY(Config)
    bool bEnableViewports = false;

    UPROPERTY(Config)
    bool bCompactMode = false;

    UPROPERTY(Config)
    bool bTransparentMode = false;

    UPROPERTY(Config)
    bool bShowHelp = true;

    UPROPERTY(Config)
    bool bShowWindowsInMainMenu = true;

    UPROPERTY(Config)
    bool bEnableInput = false;

    UPROPERTY(Config)
    bool bShareMouse = false;

    UPROPERTY(Config)
    bool bNavEnableKeyboard = false;

    UPROPERTY(Config)
    bool bNavEnableGamepad = false;

    UPROPERTY(Config)
    bool bNavNoCaptureInput = true;

    UPROPERTY(Config)
    bool bNoMouseCursorChange = false;

    virtual void Reset() override
    {
        Super::Reset();

        DPIScale = 1.0f;
        bEnableViewports = false;
        bCompactMode = false;
        bTransparentMode = false;
        bShowHelp = true;
        bShowWindowsInMainMenu = true;
        bEnableInput = false;
        bShareMouse = true;
        bNavEnableKeyboard = true;
        bNavEnableGamepad = true;
        bNavNoCaptureInput = true;
        bNoMouseCursorChange = false;
    }
};