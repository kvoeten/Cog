#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayEffect.h"
#include "CogAbilityDataAsset.generated.h"


class UGameplayEffect;

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct COGABILITY_API FCogAbilityCheat
{
    GENERATED_BODY()

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FString Name;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    TSubclassOf<UGameplayEffect> Effect;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct COGABILITY_API FCogAbilityPool
{
    GENERATED_BODY()

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FString Name;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FGameplayAttribute Value;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FGameplayAttribute Min;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FGameplayAttribute Max;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FGameplayAttribute Regen;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FLinearColor Color = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FLinearColor BackColor = FLinearColor(0.15, 0.15, 0.15, 1.0f);
};


//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogAbilityTweak
{
    GENERATED_BODY()

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FName Name;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    TSubclassOf<UGameplayEffect> Effect;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    float Multiplier = 0.01f;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    float AddPostMultiplier = 1.0f;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogAbilityTweakCategory
{
    GENERATED_BODY()

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FString Name;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FGameplayTag Id;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    TSubclassOf<AActor> ActorClass;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FGameplayTagContainer RequiredTags;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FGameplayTagContainer IgnoredTags;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FLinearColor Color = FLinearColor::White;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogAbilityTweakProfileValue
{
    GENERATED_BODY()

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    TSubclassOf<UGameplayEffect> Effect;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FGameplayTag CategoryId;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    float Value = 0.0f;
};

//--------------------------------------------------------------------------------------------------------------------------
USTRUCT()
struct FCogAbilityTweakProfile
{
    GENERATED_BODY()

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    FName Name;

    UPROPERTY(Category = "CogAbility", EditAnywhere)
    TArray<FCogAbilityTweakProfileValue> Tweaks;
};


//--------------------------------------------------------------------------------------------------------------------------
UCLASS(Blueprintable)
class COGABILITY_API UCogAbilityDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    
    UCogAbilityDataAsset() {}

    //----------------------------------------------------------------------------------------------------------------------
    // Abilities
    //----------------------------------------------------------------------------------------------------------------------

    UPROPERTY(Category = "Abilities", EditAnywhere)
    TArray<TSubclassOf<UGameplayAbility>> Abilities;

    //----------------------------------------------------------------------------------------------------------------------
    // Cheats
    //----------------------------------------------------------------------------------------------------------------------

    UPROPERTY(Category="Cheats", EditAnywhere)
    FGameplayTag NegativeEffectTag;
    
    UPROPERTY(Category = "Cheats", EditAnywhere)
    FGameplayTag PositiveEffectTag;

    UPROPERTY(Category="Cheats", EditAnywhere)
    FGameplayTag NegativeAbilityTag;
    
    UPROPERTY(Category = "Cheats", EditAnywhere)
    FGameplayTag PositiveAbilityTag;
    
    UPROPERTY(Category = "Cheats", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityCheat> PersistentEffects;

    UPROPERTY(Category = "Cheats", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityCheat> InstantEffects;

    //----------------------------------------------------------------------------------------------------------------------
    // Pools
    //----------------------------------------------------------------------------------------------------------------------

    UPROPERTY(Category = "Pools", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityPool> Pools;

    //----------------------------------------------------------------------------------------------------------------------
    // Tweaks
    //----------------------------------------------------------------------------------------------------------------------

    UPROPERTY(Category = "Tweaks", EditAnywhere)
    float TweakMinValue = -100.0f;

    UPROPERTY(Category = "Tweaks", EditAnywhere)
    float TweakMaxValue = 200.0f;

    UPROPERTY(Category = "Tweaks", EditAnywhere)
    FGameplayTag SetByCallerMagnitudeTag;

    UPROPERTY(Category = "Tweaks", EditAnywhere)
    TSubclassOf<AActor> ActorRootClass;

    UPROPERTY(Category = "Tweaks", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityTweak> Tweaks;

    UPROPERTY(Category = "Tweaks", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityTweakCategory> TweaksCategories;

    UPROPERTY(Category = "Tweaks", EditAnywhere, meta = (TitleProperty = "Name"))
    TArray<FCogAbilityTweakProfile>  TweakProfiles;
};