// Copy right GeetesNC


#include "AuraAssetManager.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/Engine.h"


UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	UAuraAssetManager* AuraAssetManager= Cast <UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
	
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FAuraGameplayTags::InitializeNativeGameplayTags();
	//This is required to use target data
	UAbilitySystemGlobals::Get().InitGlobalData();
}
/**# Widget Controller System Documentation
## Table of Contents
1. [Overview](#overview)
2. [Why We Use Widget Controllers](#why-we-use-widget-controllers)
3. [Core Components](#core-components)
4. [How It Works](#how-it-works)
5. [Implementation Guide](#implementation-guide)
6. [Best Practices](#best-practices)

## Overview
The **Widget Controller System** is an architectural pattern that acts as a **bridge between your game logic (C++) and your UI (Blueprint/UMG)**. It follows the **MVC (Model-View-Controller)** design pattern, where the Widget Controller serves as the "Controller" that manages data flow between your game systems and UI widgets.
Think of it like a translator: your game systems speak "C++ game logic," your UI speaks "Blueprint widgets," and the Widget Controller translates between them.
## Why We Use Widget Controllers
### Problems Without Widget Controllers
Without this system, you'd typically:
- **Directly reference game systems in UI widgets** → Creates tight coupling and messy code
- **Have UI logic scattered across multiple classes** → Hard to maintain and debug
- **Duplicate code across different UI widgets** → Violates DRY (Don't Repeat Yourself) principle
- **Make UI changes require C++ recompilation** → Slows down iteration time

### Benefits of Widget Controllers
✅ **Separation of Concerns**: UI doesn't need to know about game system implementation details
✅ **Reusability**: One controller can serve multiple UI widgets
✅ **Centralized Logic**: All UI data handling lives in one place
✅ **Easy Testing**: Controllers can be tested independently
✅ **Blueprint-Friendly**: Exposes only what UI designers need
✅ **Clean Architecture**: Follows SOLID principles
### Real-World Example
Imagine you're building an RPG health bar:
**❌ Without Widget Controller:**
``` 
UI Widget → Directly accesses → Character → PlayerState → AbilitySystemComponent → AttributeSet → Health Value
```
The UI needs to know about all these systems!
**✅ With Widget Controller:**
``` 
UI Widget → Listens to → Widget Controller → Manages all game system complexity
```
The UI just listens for "health changed" events!
## Core Components
### 1. FWidgetControllerParams (Configuration Struct)
**Purpose**: A convenient package that holds all the essential references needed by the Widget Controller.
**Why it exists**: Instead of passing 4+ parameters individually, we bundle them into one struct. This makes the code cleaner and easier to extend.
``` cpp
USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
    GENERATED_BODY()
    
    TObjectPtr<APlayerController> PlayerController = nullptr;
    TObjectPtr<APlayerState> PlayerState = nullptr;
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
    TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};
```
**What each reference does**:
- **PlayerController**: Handles input and player-specific actions
- **PlayerState**: Stores replicated player data (like level, XP, etc.)
- **AbilitySystemComponent**: Manages abilities, effects, and attributes (from Gameplay Ability System)
- **AttributeSet**: Holds actual attribute values (Health, Mana, Strength, etc.)

### 2. UAuraWidgetController (Base Class)
**Purpose**: The foundational controller class that all specific widget controllers inherit from.
**Key Responsibilities**:
1. **Store references** to game systems
2. **Initialize** with proper parameters
3. **Broadcast initial values** to UI when first created
4. **Bind callbacks** to listen for changes in game data
``` cpp
UCLASS()
class UAuraWidgetController : public UObject
{
    GENERATED_BODY()
    
public:
    // Called to set up the controller with necessary references
    UFUNCTION(BlueprintCallable)
    void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);
    
    // Called once to send initial data to UI
    UFUNCTION(BlueprintCallable)
    virtual void BroadcastInitialValues();
    
    // Called to start listening for changes
    virtual void BindCallbacksToDependencies();
    
protected:
    // Protected so child classes can access them
    UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
    TObjectPtr<APlayerController> PlayerController;
    
    UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
    TObjectPtr<APlayerState> PlayerState;
    
    UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
    
    UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
    TObjectPtr<UAttributeSet> AttributeSet;
};
```
## How It Works
### The Flow (Step-by-Step)
``` 
┌─────────────────────────────────────────────────────────────┐
│ 1. INITIALIZATION PHASE                                     │
├─────────────────────────────────────────────────────────────┤
│ Game System → Creates Widget Controller                     │
│            → Calls SetWidgetControllerParams()              │
│            → Controller stores references                   │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│ 2. BINDING PHASE                                            │
├─────────────────────────────────────────────────────────────┤
│ Controller → Calls BindCallbacksToDependencies()           │
│           → Subscribes to events (health changed, etc.)     │
│           → Sets up delegates/listeners                     │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│ 3. INITIAL BROADCAST                                        │
├─────────────────────────────────────────────────────────────┤
│ Controller → Calls BroadcastInitialValues()                │
│           → Reads current values from game systems          │
│           → Sends to UI (Health = 100, Mana = 50, etc.)    │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│ 4. RUNTIME UPDATES                                          │
├─────────────────────────────────────────────────────────────┤
│ Game Event (damage taken) → AbilitySystemComponent          │
│                          → Broadcasts change event          │
│                          → Controller receives it           │
│                          → Controller notifies UI           │
│                          → UI updates visuals               │
└─────────────────────────────────────────────────────────────┘
```
### Example Scenario: Displaying Health
1. **Player takes damage** in-game
2. **AbilitySystemComponent** updates the Health attribute
3. **Widget Controller** (already listening) receives the "attribute changed" callback
4. **Controller** extracts the new health value
5. **Controller** broadcasts the new value through a delegate
6. **UI Widget** (bound to the delegate) receives the new value
7. **UI** updates the health bar visual

## Implementation Guide
### Step 1: Create Your Widget Controller Class
Create a child class of `UAuraWidgetController` for specific UI needs:
``` cpp
#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

// Delegate that UI can bind to for health updates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealth);

UCLASS()
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
    GENERATED_BODY()
    
public:
    // UI widgets can bind to this in Blueprint
    UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
    FOnHealthChangedSignature OnHealthChanged;
    
    virtual void BroadcastInitialValues() override;
    virtual void BindCallbacksToDependencies() override;
    
protected:
    // Called when health attribute changes
    void HealthChanged(const FOnAttributeChangeData& Data);
};
```

``` cpp
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
    const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
    
    // Send the current health value to UI immediately
    OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
    const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
    
    // Subscribe to health changes
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        AuraAttributeSet->GetHealthAttribute()
    ).AddUObject(this, &UOverlayWidgetController::HealthChanged);
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data)
{
    // When health changes, notify all listeners (UI)
    OnHealthChanged.Broadcast(Data.NewValue);
}
```
### Step 2: Create and Initialize the Controller
Typically done in your HUD or UI manager class:
``` cpp

void AAuraHUD::InitializeOverlay(APlayerController* PC, APlayerState* PS, 
                                 UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
    // Create the widget controller
    UOverlayWidgetController* WidgetController = GetOverlayWidgetController(
        FWidgetControllerParams(PC, PS, ASC, AS)
    );
    
    // Create the UI widget
    OverlayWidget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
    
    // Connect them
    UOverlayWidget* Overlay = Cast<UOverlayWidget>(OverlayWidget);
    Overlay->SetWidgetController(WidgetController);
    
    // Initialize the controller
    WidgetController->BroadcastInitialValues();
    WidgetController->BindCallbacksToDependencies();
    
    // Add to screen
    OverlayWidget->AddToViewport();
}
```
### Step 3: Use in Blueprint UI Widget
In your UMG Widget Blueprint:
1. **Event Construct** (when widget is created):
``` 
   Event Construct
   → Get Widget Controller
   → Bind Event to OnHealthChanged
```
1. **OnHealthChanged Event**:
``` 
   OnHealthChanged (NewHealth)
   → Set Progress Bar Percent (NewHealth / MaxHealth)
   → Set Text (Format: "{0} / {1}")
```
## Best Practices
### ✅ Do's
1. **Keep controllers focused**: One controller per UI section (Overlay, Inventory, Skills, etc.)
2. **Use delegates for communication**: Never let UI directly modify game state
3. **Virtual functions**: Make `BroadcastInitialValues` and `BindCallbacksToDependencies` virtual for inheritance
4. **Null checks**: Always validate references before using them
5. **Blueprint exposure**: Use `UFUNCTION(BlueprintCallable)` for UI designer convenience

### ❌ Don'ts
1. **Don't store game logic in controllers**: They're data managers, not game systems
2. **Don't create circular dependencies**: UI → Controller → Game Systems (one-way only)
3. **Don't skip initialization**: Always call both `BroadcastInitialValues` and `BindCallbacksToDependencies`
4. **Don't update UI directly from controllers**: Use delegates/events instead

## Connection to Gameplay Ability System (GAS)
The Widget Controller system works **seamlessly with GAS**:
- **AbilitySystemComponent**: Provides the event system for attribute changes
- **AttributeSet**: Holds the actual data (Health, Mana, etc.)
- **Gameplay Effects**: When applied, they trigger attribute changes
- **Widget Controller**: Listens to these changes and notifies UI

### The FEffectAssetTags Delegate
In , you'll see: `AuraAbilitySystemComponent.h`
``` cpp
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer&)
```
This is a **custom delegate** that broadcasts whenever a Gameplay Effect is applied, sending its associated tags. Your Widget Controller can bind to this to show UI feedback (damage numbers, buff icons, etc.).
## Summary
**Widget Controllers are the communication hub between your game systems and UI**. They:
- **Centralize** UI data logic
- **Decouple** UI from complex game systems
- **Simplify** Blueprint development
- **Enable** reactive UI updates
- **Follow** clean architecture principles

By using this pattern, you create maintainable, scalable UI systems that can grow with your project without becoming a tangled mess of dependencies.
**Questions to Consider:**
- What specific UI elements are you working on? (I can provide tailored examples)
- Are you working with multiplayer? (Widget Controllers handle replication beautifully)
- Need help setting up delegates for specific attributes?
**/
