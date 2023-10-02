#pragma once

#include "CoreMinimal.h"
#include "CogDebugDefines.h"
#include "imgui.h"
#include "implot.h"

#ifdef ENABLE_COG

struct FCogDebugPlotEntry;

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugPlotEventParams
{
    FName Name;
    FString Value;
};

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugPlotEvent
{
    float GetActualEndTime(const FCogDebugPlotEntry& Plot) const;

    uint64 GetActualEndFrame(const FCogDebugPlotEntry& Plot) const;

    FCogDebugPlotEvent& AddParam(const FName Name, bool Value);

    FCogDebugPlotEvent& AddParam(const FName Name, int Value);

    FCogDebugPlotEvent& AddParam(const FName Name, float Value);

    FCogDebugPlotEvent& AddParam(const FName Name, FName Value);

    FCogDebugPlotEvent& AddParam(const FName Name, const FString& Value);

    FName Id;
    float StartTime = 0.0f;
    float EndTime = 0.0f;
    uint64 StartFrame = 0;
    uint64 EndFrame = 0;
    ImU32 BorderColor;
    ImU32 FillColor;
    int32 Row;
    FString OwnerName;
    FString DisplayName;
    TArray<FCogDebugPlotEventParams> Params;
};

//--------------------------------------------------------------------------------------------------------------------------
struct COGDEBUG_API FCogDebugPlotEntry
{
    void AssignAxis(int32 AssignedRow, ImAxis CurrentYAxis);
    void AddPoint(float X, float Y);
    bool FindValue(float Time, float& Value) const;
    void ResetAxis();
    void Clear();
    FCogDebugPlotEvent& AddEvent(const FCogDebugPlotEntry& OwnwePlot, FString OwnerName, bool IsInstant, const FName EventId, const int32 Row, const FColor& Color);
    FCogDebugPlotEvent& StopEvent(const FName EventId);
    void UpdateTime(const UWorld* World);
    int32 FindFreeRow() const;
    FCogDebugPlotEvent* GetLastEvent();
    FCogDebugPlotEvent* FindLastEventByName(FName EventId);

    FName Name;
    bool IsEventPlot = false;
    int32 CurrentRow = INDEX_NONE;
    ImAxis CurrentYAxis = ImAxis_COUNT;
    float Time = 0;
    uint64 Frame = 0;

    //--------------------------
    // Values
    //--------------------------
    int32 ValueOffset = 0;
    ImVector<ImVec2> Values;
    bool ShowValuesMarkers = false;

    //--------------------------
    // Events
    //--------------------------
    int32 EventOffset = 0;
    TArray<FCogDebugPlotEvent> Events;
    int32 MaxRow = 1;
};

//--------------------------------------------------------------------------------------------------------------------------

class COGDEBUG_API FCogDebugPlot
{
public:
    static const int32 AutoRow = -1;

    static void PlotValue(const UObject* WorldContextObject, const FName PlotName, const float Value);
    static FCogDebugPlotEvent& PlotEvent(const UObject* WorldContextObject, const FName PlotName, const FName EventId, bool IsInstant, const int32 Row = AutoRow, const FColor& Color = FColor::Transparent);
    static FCogDebugPlotEvent& PlotEventInstant(const UObject* WorldContextObject, const FName PlotName, const FName EventId, const int32 Row = AutoRow, const FColor& Color = FColor::Transparent);
    static FCogDebugPlotEvent& PlotEventStart(const UObject* WorldContextObject, const FName PlotName, const FName EventId, const int32 Row = AutoRow, const FColor& Color = FColor::Transparent);
    static FCogDebugPlotEvent& PlotEventStop(const UObject* WorldContextObject, const FName PlotName, const FName EventId);
    static FCogDebugPlotEvent& PlotEventToggle(const UObject* WorldContextObject, const FName PlotName, const FName EventId, const bool ToggleValue, const int32 Row = AutoRow, const FColor& Color = FColor::Transparent);

    static void Reset();
    static void Clear();
    static FCogDebugPlotEntry* FindPlot(const FName Name);

    static TArray<FCogDebugPlotEntry> Plots;
    static bool IsVisible;
    static bool Pause;

private:
    friend struct FCogDebugPlotEntry;

    static void ResetLastAddedEvent();
    static FCogDebugPlotEntry* RegisterPlot(const UObject* Owner, const FName PlotName, bool IsEventPlot);
    FCogDebugPlotEventParams* PlotEventAddParam(const FName Name);
    static FCogDebugPlotEvent* GetLastAddedEvent();

    static FName LastAddedEventPlotName;
    static int32 LastAddedEventIndex;

    static FCogDebugPlotEvent DefaultEvent;
};

#endif //ENABLE_COG

