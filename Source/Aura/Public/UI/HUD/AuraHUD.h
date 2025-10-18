// Copy right GeetesNC

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraHUD.generated.h"

class UOverlayWidgetController;
class UAuraUserWidget;
struct FWidgetControllerParams;
class UAttributeMenuWidgetController;
class UAbilitySystemComponent;
class UAttributeSet;




/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()
public:


	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);
	

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

	
protected:
	
	
private:

	UPROPERTY()
	TObjectPtr<UAuraUserWidget>OverlayWidget;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget>OverlayWidgetClass;
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
    UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
	

	
	
};

/**A# uraHUD - Beginner-Friendly Documentation
## What is AuraHUD?
**AuraHUD** is a custom HUD (Heads-Up Display) class that serves as a **centralized manager** for your game's user interface elements. Think of it as the "UI control center" for your game. It sits between your gameplay code and your visual UI widgets, organizing and coordinating all the UI elements that players see on screen.
## Why Do We Need AuraHUD?
### The Problem It Solves
In Unreal Engine games with complex UI systems, you typically need:
1. **Multiple UI screens** (health bars, menus, inventory, etc.)
2. **Data from various game systems** (player stats, abilities, etc.)
3. **A way to update UI** when game data changes

Without a centralized system, your code becomes messy and hard to maintain. You'd have widgets directly talking to gameplay classes, creating a tangled web of dependencies.
### The Solution: AuraHUD
AuraHUD solves this by:
- **Centralizing UI creation and management** - One place to control all your UI widgets
- **Using Widget Controllers** - Clean separation between UI visuals and game logic
- **Providing easy access** - Any system can request UI controllers through the HUD
- **Following best practices** - Implements the MVC (Model-View-Controller) pattern for UI

## Architecture Overview
``` 
Game Data (Player Stats, Abilities)
          ↓
    Widget Controllers (Logic Layer)
          ↓
      AuraHUD (Manager)
          ↓
    UI Widgets (Visual Layer)
          ↓
    Player's Screen
```
## Key Components Explained
### 1. **Widget Controllers**
Widget Controllers are the "middlemen" between your game data and UI visuals:
- **`OverlayWidgetController`** - Manages the main gameplay HUD (health, mana, XP bars)
- **`AttributeMenuWidgetController`** - Manages the attribute/stats menu

**Why use controllers?**
- Separates logic from visuals
- Reusable across different UI designs
- Easier to test and maintain
- Multiple widgets can share the same controller

### 2. **Widget Classes**
These are the actual visual elements (UMG widgets) that players see:
- **`OverlayWidget`** - The main HUD widget
- Widget controllers broadcast data changes, and widgets listen and update themselves

### 3. **Widget Controller Params**
A struct (`FWidgetControllerParams`) that packages all the necessary game systems:
- Player Controller
- Player State
- Ability System Component
- Attribute Set

This makes it easy to pass around all required dependencies at once.
## How AuraHUD Works: Step-by-Step
### Step 1: Initialization
When your game starts (usually from your Player Controller), you call:
``` cpp
InitOverlay(PC, PS, ASC, AS);
```
**What happens:**
1. Creates the Overlay Widget (the visual UI)
2. Creates the Overlay Widget Controller (the logic)
3. Adds the widget to the screen
4. Connects everything together

### Step 2: Getting Controllers
When any system needs to interact with UI:
``` cpp
UOverlayWidgetController* Controller = GetOverlayWidgetController(Params);
```
**What happens:**
1. Checks if the controller already exists
2. If not, creates a new one using the specified class
3. Initializes it with game data
4. Returns it for use

This pattern is called **lazy initialization** - controllers are only created when first needed.
### Step 3: Broadcasting Updates
Controllers listen to game events and broadcast UI updates:
``` 
Player takes damage 
    → Attribute Set updates 
    → Controller detects change 
    → Broadcasts to widgets 
    → UI updates on screen
```
## Public Functions Reference
### `InitOverlay()`
**Purpose:** Initializes and displays the main gameplay HUD
**Parameters:**
- `APlayerController* PC` - The player's controller
- `APlayerState* PS` - The player's state (stores player data)
- `UAbilitySystemComponent* ASC` - Manages abilities and attributes
- `UAttributeSet* AS` - Contains attribute values (health, mana, etc.)

**When to call:** Once, when the game starts or player spawns
**Example use case:**
``` cpp
// In your Player Controller's BeginPlay
AAuraHUD* AuraHUD = Cast<AAuraHUD>(GetHUD());
AuraHUD->InitOverlay(this, GetPlayerState(), ASC, AttributeSet);
```
### `GetOverlayWidgetController()`
**Purpose:** Gets or creates the controller for the main HUD
**Parameters:**
- `const FWidgetControllerParams& WCParams` - Package of required game systems

**Returns:** Pointer to the Overlay Widget Controller
**When to call:** Whenever you need to update the main HUD
**Example use case:**
``` cpp
// Broadcasting a health change
FWidgetControllerParams Params(PC, PS, ASC, AS);
UOverlayWidgetController* Controller = HUD->GetOverlayWidgetController(Params);
Controller->BroadcastInitialValues();
```
### `GetAttributeMenuWidgetController()`
**Purpose:** Gets or creates the controller for the attribute menu
**Parameters:**
- `const FWidgetControllerParams& WCParams` - Package of required game systems

**Returns:** Pointer to the Attribute Menu Widget Controller
**When to call:** When opening the attribute/stats menu
**Example use case:**
``` cpp
// Opening the attribute menu
FWidgetControllerParams Params(PC, PS, ASC, AS);
UAttributeMenuWidgetController* Controller = HUD->GetAttributeMenuWidgetController(Params);
// Controller now ready to populate the menu
```
## Understanding the Private Variables
### Widget Instances
``` cpp
UPROPERTY()
TObjectPtr<UAuraUserWidget> OverlayWidget;
```
- Stores the **actual widget instance** currently on screen
- Created once during `InitOverlay()`

### Widget Classes
``` cpp
UPROPERTY(EditAnywhere)
TSubclassOf<UAuraUserWidget> OverlayWidgetClass;
```
- Stores the **blueprint class** to create instances from
- Set in the Blueprint editor
- Used as a template for creating widgets

### Controller Instances
``` cpp
UPROPERTY()
TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
```
- Stores the **controller instance**
- Created on first access (lazy initialization)
- Reused for subsequent calls

### Controller Classes
``` cpp
UPROPERTY(EditAnywhere)
TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
```
- Stores the **blueprint class** for the controller
- Set in the Blueprint editor
- Used as a template for creating controllers

## Workflow Example: Complete Picture
### Scenario: Displaying Player Health
1. **Setup (In Blueprint):**
    - Create a Blueprint based on `AAuraHUD`
    - Assign your UI widget blueprints to `OverlayWidgetClass`
    - Assign your controller blueprint to `OverlayWidgetControllerClass`

2. **Initialization (In C++):**
``` cpp
   // Player Controller calls this on spawn
   AuraHUD->InitOverlay(PC, PS, ASC, AS);
   // Now health bar is on screen, ready to display data
```
1. **Updates (Automatic):**
``` cpp
   // Player takes damage
   // Attribute Set changes health value
   // Controller has callbacks listening to Attribute Set
   // Controller broadcasts new health value
   // Widget receives broadcast and updates health bar visually
```
1. **Accessing from Other Systems:**
``` cpp
   // A quest system wants to highlight the health bar
   AAuraHUD* HUD = PlayerController->GetHUD<AAuraHUD>();
   UOverlayWidgetController* Controller = HUD->GetOverlayWidgetController(Params);
   Controller->HighlightHealthBar();
```
## Common Patterns & Best Practices
### ✅ Do's
1. **Create a Blueprint from AuraHUD** - Don't use the C++ class directly
2. **Set all widget/controller classes in Blueprint** - Use the EditAnywhere properties
3. **Initialize once** - Call `InitOverlay()` only once per game session
4. **Use controllers for logic** - Never put game logic directly in widgets
5. **Cache controller references** - Get the controller once and reuse it

### ❌ Don'ts
1. **Don't create widgets manually** - Let the HUD manage creation
2. **Don't skip initialization** - Always call `InitOverlay()` before using the HUD
3. **Don't access game data from widgets** - Use controllers as intermediaries
4. **Don't create multiple HUD instances** - One HUD per player

## Troubleshooting
### "Widget not showing on screen"
- Check if `InitOverlay()` was called
- Verify `OverlayWidgetClass` is set in Blueprint
- Ensure the widget is designed in UMG Designer

### "Controller returns null"
- Check if `OverlayWidgetControllerClass` is set in Blueprint
- Verify you're passing valid `FWidgetControllerParams`

### "UI not updating"
- Ensure the controller is properly initialized with ASC and Attribute Set
- Check if callbacks are bound in the controller
- Verify the widget is listening to controller broadcasts

## Summary
**AuraHUD** is your UI management hub. It:
- ✅ Creates and manages UI widgets
- ✅ Provides access to widget controllers
- ✅ Initializes the connection between game data and UI
- ✅ Keeps your code organized and maintainable

By centralizing UI management, AuraHUD makes it easy to add, modify, and maintain complex user interfaces in your Unreal Engine project!
**/
