#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "GameFramework/Actor.h"
#include "CogEngineCollisionTester.generated.h"

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogEngine_CollisionQueryType : uint8
{
    Overlap,
    LineTrace,
    Sweep,
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogEngine_CollisionQueryTraceMode : uint8
{
    Single,
    Multi,
    Test,
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogEngine_CollisionQueryOverlapMode : uint8
{
    AnyTest,
    BlockingTest,
    Multi,
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogEngine_CollisionQueryBy : uint8
{
    Channel,
    ObjectType,
    Profile,
};

//--------------------------------------------------------------------------------------------------------------------------
UENUM()
enum class ECogEngine_CollisionQueryShape : uint8
{
    Sphere,
    Box,
    Capsule,
};

//--------------------------------------------------------------------------------------------------------------------------
UCLASS()
class COGENGINE_API ACogEngineCollisionTester : public AActor
{
    GENERATED_BODY()

public:
    ACogEngineCollisionTester(const FObjectInitializer& ObjectInitializer);

    virtual void Tick(float DeltaSeconds) override;

    virtual bool ShouldTickIfViewportsOnly() const override;

    void Query() const;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    bool TickInEditor = false;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    ECogEngine_CollisionQueryType Type = ECogEngine_CollisionQueryType::LineTrace;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    ECogEngine_CollisionQueryTraceMode TraceMode = ECogEngine_CollisionQueryTraceMode::Multi;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    ECogEngine_CollisionQueryOverlapMode OverlapMode = ECogEngine_CollisionQueryOverlapMode::Multi;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    ECogEngine_CollisionQueryBy By = ECogEngine_CollisionQueryBy::Channel;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    ECogEngine_CollisionQueryShape Shape = ECogEngine_CollisionQueryShape::Sphere;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    bool TraceComplex = false;

    UPROPERTY()
    int32 ObjectTypesToQuery = 0;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

    UPROPERTY()
    int32 ProfileIndex = 0;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    FVector ShapeExtent = FVector(100, 100, 100);

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    bool DrawHitLocations = true;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    bool DrawHitImpactPoints = true;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    bool DrawHitShapes = true;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    bool DrawHitNormals = true;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    bool DrawHitImpactNormals = true;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    bool DrawHitPrimitives = true;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    bool DrawHitActorsNames = false;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    float HitPointSize = 5.0f;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    FColor NoHitColor = FColor::Red;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    FColor HitColor = FColor::Green;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    FColor NormalColor = FColor::Yellow;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    FColor ImpactNormalColor = FColor::Cyan;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    TObjectPtr<USceneComponent> StartComponent;

    UPROPERTY(Category = "CogEngine", EditAnywhere)
    TObjectPtr<USceneComponent> EndComponent;
};
