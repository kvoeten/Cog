#include "CogImGuiContext.h"

#include "CogImguiHelper.h"
#include "CogImguiInputHelper.h"
#include "CogImguiInputProcessor.h"
#include "CogImguiWidget.h"
#include "Engine/Console.h"
#include "Engine/LocalPlayer.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/SlateUser.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "TextureResource.h"
#include "Widgets/SViewport.h"
#include "Widgets/SWindow.h"

static UPlayerInput* GetPlayerInput(const UWorld* World);

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::Initialize()
{
    IMGUI_CHECKVERSION();

    if (FSlateApplication::IsInitialized() == false)
    {
        return;
    }
    FSlateApplication& SlateApp = FSlateApplication::Get();

    GameViewport = GEngine->GameViewport;

    SAssignNew(MainWidget, SCogImguiWidget)
              .Context(this);

    GameViewport->AddViewportWidgetContent(MainWidget.ToSharedRef(), TNumericLimits<int32>::Max());
    
    //--------------------------------------------------------------------
    // Register input processor to forward input events to imgui
    //--------------------------------------------------------------------
    if (FSlateApplication::IsInitialized())
    {
        UPlayerInput* PlayerInput = FCogImguiInputHelper::GetPlayerInput(*GameViewport->GetWorld());
        InputProcessor = MakeShared<FImGuiInputProcessor>(PlayerInput, this, MainWidget.Get());
        FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor.ToSharedRef(), 0);
    }

    ImGuiContext = ImGui::CreateContext();
    PlotContext = ImPlot::CreateContext();
    ImPlot::SetImGuiContext(ImGuiContext);

    ImGuiIO& IO = ImGui::GetIO();
    IO.UserData = this;

    IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    IO.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
    IO.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    IO.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    IO.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    IO.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
    IO.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;
    IO.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
    IO.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    //--------------------------------------------------------------------
    // 
    //--------------------------------------------------------------------
    ImGuiViewport* MainViewport = ImGui::GetMainViewport();
    FImGuiViewportData* ViewportData = new FImGuiViewportData();
    MainViewport->PlatformUserData = ViewportData;
    ViewportData->Window = SlateApp.GetActiveTopLevelWindow();
    ViewportData->Context = this;
    ViewportData->Widget = MainWidget;

    const char* InitFilenameTemp = TCHAR_TO_ANSI(*FCogImguiHelper::GetIniFilePath("imgui"));
    ImStrncpy(IniFilename, InitFilenameTemp, IM_ARRAYSIZE(IniFilename));
    IO.IniFilename = IniFilename;

    ImGuiPlatformIO& PlatformIO = ImGui::GetPlatformIO();
    PlatformIO.Platform_CreateWindow = ImGui_CreateWindow;
    PlatformIO.Platform_DestroyWindow = ImGui_DestroyWindow;
    PlatformIO.Platform_ShowWindow = ImGui_ShowWindow;
    PlatformIO.Platform_SetWindowPos = ImGui_SetWindowPos;
    PlatformIO.Platform_GetWindowPos = ImGui_GetWindowPos;
    PlatformIO.Platform_SetWindowSize = ImGui_SetWindowSize;
    PlatformIO.Platform_GetWindowSize = ImGui_GetWindowSize;
    PlatformIO.Platform_SetWindowFocus = ImGui_SetWindowFocus;
    PlatformIO.Platform_GetWindowFocus = ImGui_GetWindowFocus;
    PlatformIO.Platform_GetWindowMinimized = ImGui_GetWindowMinimized;
    PlatformIO.Platform_SetWindowTitle = ImGui_SetWindowTitle;
    PlatformIO.Platform_SetWindowAlpha = ImGui_SetWindowAlpha;
    PlatformIO.Platform_RenderWindow = ImGui_RenderWindow;

    if (const TSharedPtr<GenericApplication> PlatformApplication = SlateApp.GetPlatformApplication())
    {
        FDisplayMetrics DisplayMetrics;
        PlatformApplication->GetInitialDisplayMetrics(DisplayMetrics);
        PlatformApplication->OnDisplayMetricsChanged().AddRaw(this, &FCogImguiContext::OnDisplayMetricsChanged);
        OnDisplayMetricsChanged(DisplayMetrics);
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::Shutdown()
{
    ImGuiViewport* MainViewport = ImGui::GetMainViewport();
    if (const FImGuiViewportData* ViewportData = static_cast<FImGuiViewportData*>(MainViewport->PlatformUserData))
    {
        delete ViewportData;
        MainViewport->PlatformUserData = nullptr;
    }

    if (FSlateApplication::IsInitialized())
    {
        FSlateApplication& SlateApp = FSlateApplication::Get();

        if (InputProcessor.IsValid())
        {
            SlateApp.UnregisterInputPreProcessor(InputProcessor);
        }

        if (const TSharedPtr<GenericApplication> PlatformApplication = SlateApp.GetPlatformApplication())
        {
            PlatformApplication->OnDisplayMetricsChanged().RemoveAll(this);
        }
    }

    if (PlotContext)
    {
        ImPlot::DestroyContext(PlotContext);
        PlotContext = nullptr;
    }

    if (ImGuiContext)
    {
        ImGui::DestroyContext(ImGuiContext);
        ImGuiContext = nullptr;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::OnDisplayMetricsChanged(const FDisplayMetrics& DisplayMetrics) const
{
    ImGuiPlatformIO& PlatformIO = ImGui::GetPlatformIO();
    PlatformIO.Monitors.resize(0);

    for (const FMonitorInfo& Monitor : DisplayMetrics.MonitorInfo)
    {
        ImGuiPlatformMonitor ImGuiMonitor;
        ImGuiMonitor.MainPos = ImVec2(Monitor.DisplayRect.Left, Monitor.DisplayRect.Top);
        ImGuiMonitor.MainSize = ImVec2(Monitor.DisplayRect.Right - Monitor.DisplayRect.Left, Monitor.DisplayRect.Bottom - Monitor.DisplayRect.Top);
        ImGuiMonitor.WorkPos = ImVec2(Monitor.WorkArea.Left, Monitor.WorkArea.Top);
        ImGuiMonitor.WorkSize = ImVec2(Monitor.WorkArea.Right - Monitor.WorkArea.Left, Monitor.WorkArea.Bottom - Monitor.WorkArea.Top);
        ImGuiMonitor.DpiScale = Monitor.DPI;

        if (Monitor.bIsPrimary)
        {
            PlatformIO.Monitors.push_front(ImGuiMonitor);
        }
        else
        {
            PlatformIO.Monitors.push_back(ImGuiMonitor);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::BeginFrame(float InDeltaTime)
{
    ImGui::SetCurrentContext(ImGuiContext);
    ImPlot::SetImGuiContext(ImGuiContext);
    ImPlot::SetCurrentContext(PlotContext);

    ImGuiIO& IO = ImGui::GetIO();
    IO.DeltaTime = InDeltaTime;
    IO.DisplaySize = FCogImguiHelper::ToImVec2(MainWidget->GetTickSpaceGeometry().GetAbsoluteSize());

    //-------------------------------------------------------------------------------------------------------
    // Build font
    //-------------------------------------------------------------------------------------------------------
    if (IO.Fonts->IsBuilt() == false || FontAtlasTexturePtr.IsValid() == false)
    {
        BuildFont();
    }

    //-------------------------------------------------------------------------------------------------------
    // Update which viewport is under the mouse
    //-------------------------------------------------------------------------------------------------------
    ImGuiID MouseViewportId = 0;

    FSlateApplication& SlateApp = FSlateApplication::Get();
    FWidgetPath WidgetsUnderCursor = SlateApp.LocateWindowUnderMouse(SlateApp.GetCursorPos(), SlateApp.GetInteractiveTopLevelWindows());
    if (WidgetsUnderCursor.IsValid())
    {
        TSharedRef<SWindow> Window = WidgetsUnderCursor.GetWindow();
        ImGuiID* ViewportId = WindowToViewportMap.Find(Window);

        if (ViewportId != nullptr)
        {
            MouseViewportId = *ViewportId;
        }
        else
        {
            MouseViewportId = ImGui::GetMainViewport()->ID;
        }
    }

    IO.AddMouseViewportEvent(MouseViewportId);

    //-------------------------------------------------------------------------------------------------------
    // Refresh modifiers otherwise, when pressing ALT-TAB, the Alt modifier is always true
    //-------------------------------------------------------------------------------------------------------
    FModifierKeysState ModifierKeys = FSlateApplication::Get().GetModifierKeys();
    if (ModifierKeys.IsControlDown() != IO.KeyCtrl) { IO.AddKeyEvent(ImGuiMod_Ctrl, ModifierKeys.IsControlDown()); }
    if (ModifierKeys.IsShiftDown() != IO.KeyShift) { IO.AddKeyEvent(ImGuiMod_Shift, ModifierKeys.IsShiftDown()); }
    if (ModifierKeys.IsAltDown() != IO.KeyAlt) { IO.AddKeyEvent(ImGuiMod_Alt, ModifierKeys.IsAltDown()); }
    if (ModifierKeys.IsCommandDown() != IO.KeySuper) { IO.AddKeyEvent(ImGuiMod_Super, ModifierKeys.IsCommandDown()); }


    //-------------------------------------------------------------------------------------------------------
    // 
    //-------------------------------------------------------------------------------------------------------
    if (bEnableInput)
    {
        IO.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;

        TryReleaseGameMouseCapture();
    }
    else
    {
        IO.ConfigFlags |= ImGuiConfigFlags_NoMouse;
    }

    //-------------------------------------------------------------------------------------------------------
    // 
    //-------------------------------------------------------------------------------------------------------
    const bool bHasMouse = (IO.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0;
    const bool bUpdateMouseMouseCursor = (IO.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0;
    if (bHasMouse && bUpdateMouseMouseCursor)
    {
        MainWidget->SetCursor(FCogImguiInputHelper::ToSlateMouseCursor(ImGui::GetMouseCursor()));
    }

    //-------------------------------------------------------------------------------------------------------
    // 
    //-------------------------------------------------------------------------------------------------------
    if (bRefreshDPIScale)
    {
        bRefreshDPIScale = false;

        BuildFont();

        ImGuiStyle NewStyle = ImGuiStyle();
        ImGui::GetStyle() = MoveTemp(NewStyle);
        NewStyle.ScaleAllSizes(DpiScale);
    }

    ImGui::NewFrame();

    //DrawDebug();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::EndFrame()
{
    ImGui::Render();
    ImGui_RenderWindow(ImGui::GetMainViewport(), nullptr);

    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_CreateWindow(ImGuiViewport* Viewport)
{
    if (Viewport->ParentViewportId == 0)
    {
        return;
    }

    ImGuiViewport* ParentViewport = ImGui::FindViewportByID(Viewport->ParentViewportId);
    if (ParentViewport == nullptr)
    {
        return;
    }

    const FImGuiViewportData* ParentViewportData = static_cast<FImGuiViewportData*>(ParentViewport->PlatformUserData);
    if (ParentViewportData == nullptr)
    {
        return;
    }

    FCogImguiContext* Context = ParentViewportData->Context.Get();

    const bool bTooltipWindow = (Viewport->Flags & ImGuiViewportFlags_TopMost);
    const bool bPopupWindow = (Viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon);

    static FWindowStyle WindowStyle = FWindowStyle()
        .SetActiveTitleBrush(FSlateNoResource())
        .SetInactiveTitleBrush(FSlateNoResource())
        .SetFlashTitleBrush(FSlateNoResource())
        .SetOutlineBrush(FSlateNoResource())
        .SetBorderBrush(FSlateNoResource())
        .SetBackgroundBrush(FSlateNoResource())
        .SetChildBackgroundBrush(FSlateNoResource());

    TSharedPtr<SCogImguiWidget> Widget;

    const TSharedRef<SWindow> Window =
        SNew(SWindow)
        .Type(bTooltipWindow ? EWindowType::ToolTip : EWindowType::Normal)
        .Style(&WindowStyle)
        .ScreenPosition(FCogImguiHelper::ToFVector2D(Viewport->Pos))
        .ClientSize(FCogImguiHelper::ToFVector2D(Viewport->Size))
        .SupportsTransparency(EWindowTransparency::PerWindow)
        .SizingRule(ESizingRule::UserSized)
        .IsPopupWindow(bTooltipWindow || bPopupWindow)
        .IsTopmostWindow(bTooltipWindow)
        .FocusWhenFirstShown(!(Viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing))
        .HasCloseButton(false)
        .SupportsMaximize(false)
        .SupportsMinimize(false)
        .CreateTitleBar(false)
        .LayoutBorder(0)
        .UserResizeBorder(0)
        .UseOSWindowBorder(false)
        [
            SAssignNew(Widget, SCogImguiWidget)
                      .Context(Context)
        ];

    if (ParentViewportData->Window.IsValid())
    {
        FSlateApplication::Get().AddWindowAsNativeChild(Window, ParentViewportData->Window.Pin().ToSharedRef());
    }
    else
    {
        FSlateApplication::Get().AddWindow(Window);
    }

    FImGuiViewportData* ViewportData = new FImGuiViewportData();
    Viewport->PlatformUserData = ViewportData;
    ViewportData->Context = ParentViewportData->Context;
    ViewportData->Widget = Widget;
    ViewportData->Window = Window;

    ParentViewportData->Context->WindowToViewportMap.Add(Window, Viewport->ID);

    Viewport->PlatformRequestResize = false;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_DestroyWindow(ImGuiViewport* Viewport)
{
    FImGuiViewportData* ViewportData = static_cast<FImGuiViewportData*>(Viewport->PlatformUserData);
    if (ViewportData == nullptr)
    {
        return;
    }

    if ((Viewport->Flags & ImGuiViewportFlags_OwnedByApp))
    {
        return;
    }

    ViewportData->Context->WindowToViewportMap.Remove(ViewportData->Window);

    if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
    {
        Window->RequestDestroyWindow();
    }

    delete ViewportData;
    Viewport->PlatformUserData = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_ShowWindow(ImGuiViewport* Viewport)
{
    if (const FImGuiViewportData* ViewportData = static_cast<FImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            Window->ShowWindow();
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_SetWindowPos(ImGuiViewport* Viewport, ImVec2 Pos)
{
    if (const FImGuiViewportData* ViewportData = static_cast<FImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            Window->MoveWindowTo(FCogImguiHelper::ToFVector2D(Pos));
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec2 FCogImguiContext::ImGui_GetWindowPos(ImGuiViewport* Viewport)
{
    if (const FImGuiViewportData* ViewportData = static_cast<FImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SCogImguiWidget> Widget = ViewportData->Widget.Pin())
        {
            return FCogImguiHelper::ToImVec2(Widget->GetTickSpaceGeometry().GetAbsolutePosition());
        }
    }

    return ImVec2(0, 0);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_SetWindowSize(ImGuiViewport* Viewport, ImVec2 Size)
{
    if (const FImGuiViewportData* ViewportData = static_cast<FImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            Window->Resize(FCogImguiHelper::ToFVector2D(Size));
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
ImVec2 FCogImguiContext::ImGui_GetWindowSize(ImGuiViewport* Viewport)
{
    if (const FImGuiViewportData* ViewportData = static_cast<FImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SCogImguiWidget> Widget = ViewportData->Widget.Pin())
        {
            return FCogImguiHelper::ToImVec2(Widget->GetTickSpaceGeometry().GetAbsoluteSize());
        }
    }

    return ImVec2(0, 0);
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_SetWindowFocus(ImGuiViewport* Viewport)
{
    if (const FImGuiViewportData* ViewportData = static_cast<FImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            if (const TSharedPtr<FGenericWindow> NativeWindow = Window->GetNativeWindow())
            {
                NativeWindow->BringToFront();
                NativeWindow->SetWindowFocus();
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiContext::ImGui_GetWindowFocus(ImGuiViewport* Viewport)
{
    if (const FImGuiViewportData* ViewportData = static_cast<FImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            if (const TSharedPtr<FGenericWindow> NativeWindow = Window->GetNativeWindow())
            {
                return NativeWindow->IsForegroundWindow();
            }
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiContext::ImGui_GetWindowMinimized(ImGuiViewport* Viewport)
{
    if (const FImGuiViewportData* ViewportData = static_cast<FImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            return Window->IsWindowMinimized();
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_SetWindowTitle(ImGuiViewport* Viewport, const char* TitleAnsi)
{
    if (const FImGuiViewportData* ViewportData = static_cast<FImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            Window->SetTitle(FText::FromString(ANSI_TO_TCHAR(TitleAnsi)));
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_SetWindowAlpha(ImGuiViewport* Viewport, float Alpha)
{
    if (const FImGuiViewportData* ViewportData = static_cast<FImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SWindow> Window = ViewportData->Window.Pin())
        {
            Window->SetOpacity(Alpha);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::ImGui_RenderWindow(ImGuiViewport* Viewport, void* Data)
{
    if (const FImGuiViewportData* ViewportData = static_cast<FImGuiViewportData*>(Viewport->PlatformUserData))
    {
        if (const TSharedPtr<SCogImguiWidget> Widget = ViewportData->Widget.Pin())
        {
            Widget->SetDrawData(Viewport->DrawData);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------
static UPlayerInput* GetPlayerInput(const UWorld* World)
{
    if (World == nullptr)
    {
        return nullptr;
    }

    APlayerController* PlayerController = nullptr;
    for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* ItPlayerController = Iterator->Get();
        if (ItPlayerController->IsLocalController())
        {
            PlayerController = ItPlayerController;
            break;
        }
    }

    if (PlayerController == nullptr)
    {
        return nullptr;
    }

    UPlayerInput* PlayerInput = PlayerController->PlayerInput;
    return PlayerInput;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::SetEnableInput(bool Value)
{
    bEnableInput = Value; 

    if (bEnableInput == false)
    {
        TryGiveMouseCaptureBackToGame();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::SetShareMouse(bool Value)
{
    bShareMouse = Value; 

    if (bEnableInput == false)
    {
        TryGiveMouseCaptureBackToGame();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::TryReleaseGameMouseCapture()
{
    if (bShareMouse)
    {
        return;
    }

    if (TSharedPtr<FSlateUser> User = FSlateApplication::Get().GetCursorUser())
    {
        if (User->HasCursorCapture())
        {
            PreviousMouseCaptor = User->GetCursorCaptor();
        }
    }

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        LocalPlayer->GetSlateOperations()
            .ReleaseMouseLock()
            .ReleaseMouseCapture();
    }
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::TryGiveMouseCaptureBackToGame()
{
    if (PreviousMouseCaptor.IsValid() == false)
    {
        return;
    }
 
    TSharedRef<SWidget> PreviousMouseCaptorRef = PreviousMouseCaptor.Pin().ToSharedRef();

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        LocalPlayer->GetSlateOperations().CaptureMouse(PreviousMouseCaptorRef);
    }

    PreviousMouseCaptor.Reset();
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::SetDPIScale(float Value)
{
    if (DpiScale == Value)
    {
        return;
    }

    DpiScale = Value;
    bRefreshDPIScale = true;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::BuildFont()
{
    ImGuiIO& IO = ImGui::GetIO();
    IO.Fonts->Clear();

    ImFontConfig FontConfig;
    FontConfig.SizePixels = FMath::RoundFromZero(13.f * DpiScale);
    IO.Fonts->AddFontDefault(&FontConfig);

    uint8* TextureDataRaw;
    int32 TextureWidth, TextureHeight, BytesPerPixel;
    IO.Fonts->GetTexDataAsRGBA32(&TextureDataRaw, &TextureWidth, &TextureHeight, &BytesPerPixel);

    UTexture2D* FontAtlasTexture = UTexture2D::CreateTransient(TextureWidth, TextureHeight, PF_R8G8B8A8, TEXT("ImGuiFontAtlas"));
    FontAtlasTexture->Filter = TF_Bilinear;
    FontAtlasTexture->AddressX = TA_Wrap;
    FontAtlasTexture->AddressY = TA_Wrap;

    uint8* FontAtlasTextureData = static_cast<uint8*>(FontAtlasTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
    FMemory::Memcpy(FontAtlasTextureData, TextureDataRaw, TextureWidth * TextureHeight * BytesPerPixel);
    FontAtlasTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
    FontAtlasTexture->UpdateResource();

    IO.Fonts->SetTexID(FontAtlasTexture);
    FontAtlasTexturePtr.Reset(FontAtlasTexture);
}

//--------------------------------------------------------------------------------------------------------------------------
bool FCogImguiContext::IsConsoleOpened() const
{
    return GameViewport->ViewportConsole && GameViewport->ViewportConsole->ConsoleState != NAME_None;
}

//--------------------------------------------------------------------------------------------------------------------------
void FCogImguiContext::DrawDebug()
{
    if (ImGui::Begin("ImGui Integration Debug"))
    {
        ImGui::BeginDisabled();

        ImVec2 AbsPos = FCogImguiHelper::ToImVec2(MainWidget->GetTickSpaceGeometry().GetAbsolutePosition());
        ImGui::InputFloat2("Widget Abs Pos", &AbsPos.x, "%0.1f");

        ImVec2 AbsSize = FCogImguiHelper::ToImVec2(MainWidget->GetTickSpaceGeometry().GetAbsoluteSize());
        ImGui::InputFloat2("Widget Abs Size", &AbsSize.x, "%0.1f");

        ImVec2 LocalSize = FCogImguiHelper::ToImVec2(MainWidget->GetTickSpaceGeometry().GetLocalSize());
        ImGui::InputFloat2("Widget Local Size", &LocalSize.x, "%0.1f");

        FSlateApplication& SlateApp = FSlateApplication::Get();
        ImVec2 MousePosition = FCogImguiHelper::ToImVec2(SlateApp.GetCursorPos());
        ImGui::InputFloat2("Mouse", &MousePosition.x, "%0.1f");

        ImGuiIO& IO = ImGui::GetIO();
        ImGui::InputFloat2("ImGui Mouse", &IO.MousePos.x, "%0.1f");

        FString Focus = "None";
        if (TSharedPtr<SWidget> KeyboardFocusedWidget = SlateApp.GetKeyboardFocusedWidget())
        {
            Focus = KeyboardFocusedWidget->ToString();
        }
        static char Buffer[256] = "";
        ImStrncpy(Buffer, TCHAR_TO_ANSI(*Focus), IM_ARRAYSIZE(Buffer));
        ImGui::InputText("Keyboard Focus", Buffer, IM_ARRAYSIZE(Buffer));

        ImGui::EndDisabled();
    }
    ImGui::End();
}

//--------------------------------------------------------------------------------------------------------------------------
ULocalPlayer* FCogImguiContext::GetLocalPlayer() const
{
    if (GameViewport == nullptr)
    {
        return nullptr;
    }

    UWorld* World = GameViewport->GetWorld();
    if (World == nullptr)
    {
        return nullptr;
    }

    ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
    return LocalPlayer;
}
