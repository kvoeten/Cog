#pragma  once

#include "CoreMinimal.h"
#include "CogCommonLog.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "CogDebugLogBlueprint.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UCLASS(meta = (ScriptName = "CogLogBlueprint"))
class COGDEBUG_API UCogDebugLogBlueprint : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(Category = "CogDebug", BlueprintCallable, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static void Log(const UObject* WorldContextObject, FCogLogCategory LogCategory, ECogLogVerbosity Verbosity = ECogLogVerbosity::Verbose, const FString& Text = FString(""));

    UFUNCTION(Category = "CogDebug", BlueprintPure, meta = (DevelopmentOnly, WorldContext = "WorldContextObject"))
    static bool IsLogActive(const UObject* WorldContextObject, const FCogLogCategory LogCategory);

};