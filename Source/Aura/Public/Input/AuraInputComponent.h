// Copy right GeetesNC

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Input/AuraInputConfig.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserClass, typename  PressedFuncType, typename  ReleasedFuncType, typename  HeldFuncType>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
	
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
check(InputConfig)

	for (const FAuraInputAction& Action:InputConfig->AbilityInputActions)
	{
		if (Action.InputAction&&Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}

			if (ReleasedFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
				
			}

			
			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
		}
			
	} 
	
}

/**
*# AuraInputComponent Documentation
## What is AuraInputComponent?
`UAuraInputComponent` is a **custom input component** for Unreal Engine that extends the Enhanced Input System. It simplifies the process of binding multiple gameplay abilities to input actions, making it much easier to set up complex input systems for games with multiple abilities (like RPGs, MOBAs, or action games).
## Why Use AuraInputComponent?
### The Problem It Solves
In games with multiple abilities (think of a game like League of Legends or World of Warcraft), you typically need to:
- Bind many different inputs (Q, E, R, mouse clicks, etc.)
- Handle different input states for each ability (pressed, released, held)
- Associate each input with a specific gameplay tag or ability identifier

Doing this manually for every ability would require writing repetitive binding code like:
``` cpp
// Without AuraInputComponent - repetitive and error-prone
BindAction(QAbilityInput, ETriggerEvent::Started, this, &AMyCharacter::OnQPressed, QTag);
BindAction(QAbilityInput, ETriggerEvent::Completed, this, &AMyCharacter::OnQReleased, QTag);
BindAction(QAbilityInput, ETriggerEvent::Triggered, this, &AMyCharacter::OnQHeld, QTag);
// ... repeat for every ability
```
### The Solution
`AuraInputComponent` provides a **single function** that automatically sets up all these bindings for all your abilities at once, using configuration data. This means:
✅ **Less code** - One function call instead of dozens of binding statements
✅ **Less error-prone** - Configure inputs in data assets, not hardcoded in C++
✅ **More maintainable** - Easy to add new abilities without modifying binding code
✅ **Designer-friendly** - Input configurations can be set up in the Unreal Editor
## How It Works
### Overview
The component works with three main pieces:
1. **AuraInputComponent** - The custom input component (this class)
2. **AuraInputConfig** - A data asset that stores which inputs map to which abilities
3. **Your Character/Controller** - Where you define what happens when abilities are triggered

### Key Concepts
**Input States**: The component handles three input states for each ability:
- **Pressed** - When the key/button is first pressed down (Started)
- **Released** - When the key/button is let go (Completed)
- **Held** - While the key/button is being held down (Triggered continuously)

**Gameplay Tags**: Each ability is identified by a Gameplay Tag, allowing integration with Unreal's Gameplay Ability System (GAS).
## How to Use AuraInputComponent
### Step 1: Set Up Your Input Config Data Asset
First, create a data asset of type `UAuraInputConfig` that defines your ability inputs:
``` cpp
// In the editor, this would contain entries like:
// Ability 1: Input Action = IA_Ability1, Input Tag = "Abilities.Fire"
// Ability 2: Input Action = IA_Ability2, Input Tag = "Abilities.Ice"
// etc.
```
### Step 2: Create Callback Functions
In your character or controller class, define the functions that will be called for each input state:
``` cpp
// In your header file
UCLASS()
class AMyCharacter : public ACharacter
{
    GENERATED_BODY()

protected:
    void AbilityInputPressed(FGameplayTag InputTag);
    void AbilityInputReleased(FGameplayTag InputTag);
    void AbilityInputHeld(FGameplayTag InputTag);
};
```

``` cpp
// In your .cpp file
void AMyCharacter::AbilityInputPressed(FGameplayTag InputTag)
{
    // Handle when an ability input is first pressed
    // InputTag tells you which ability (e.g., "Abilities.Fire")
}

void AMyCharacter::AbilityInputReleased(FGameplayTag InputTag)
{
    // Handle when an ability input is released
}

void AMyCharacter::AbilityInputHeld(FGameplayTag InputTag)
{
    // Handle while an ability input is held down
    // This gets called every frame while held
}
```
### Step 3: Bind Everything Together
In your `SetupPlayerInputComponent` function, use `BindAbilityActions`:
``` cpp
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Cast to AuraInputComponent
    UAuraInputComponent* AuraInputComp = Cast<UAuraInputComponent>(PlayerInputComponent);
    
    if (AuraInputComp && AbilityInputConfig) // AbilityInputConfig is your data asset
    {
        // One function call binds ALL your abilities!
        AuraInputComp->BindAbilityActions(
            AbilityInputConfig,
            this,
            &AMyCharacter::AbilityInputPressed,
            &AMyCharacter::AbilityInputReleased,
            &AMyCharacter::AbilityInputHeld
        );
    }
}
```
### Step 4: Configure Your Project to Use AuraInputComponent
Make sure your player controller or character uses `UAuraInputComponent` as its input component class. You can set this in the constructor:
``` cpp
AMyCharacter::AMyCharacter()
{
    // Tell Unreal to use AuraInputComponent instead of the default
    static ConstructorHelpers::FClassFinder<UInputComponent> InputComponentClassFinder(
        TEXT("/Script/Aura.AuraInputComponent")
    );
    
    if (InputComponentClassFinder.Succeeded())
    {
        InputComponentClass = InputComponentClassFinder.Class;
    }
}
```
## Complete Example Flow
1. Player presses the "Q" key
2. Enhanced Input System detects this as "IA_FireAbility" input action
3. AuraInputComponent looks up "IA_FireAbility" and finds it's tagged with "Abilities.Fire"
4. Your `AbilityInputPressed` function is called with the tag "Abilities.Fire"
5. You can then activate the appropriate ability based on the tag

## Tips for Beginners
- **Start Simple**: Begin with just 2-3 abilities to understand the system
- **Use the Editor**: Create your Input Config data asset in the editor, not code
- **Debug with Logs**: Add `UE_LOG` statements in your callback functions to see which abilities are being triggered
- **Optional Callbacks**: You can pass `nullptr` for any callback you don't need (e.g., if abilities don't need "held" behavior)

## Summary
`AuraInputComponent` is a time-saving utility that automates the tedious process of binding multiple ability inputs. Instead of manually binding each ability's pressed/released/held states, you configure your abilities in a data asset and bind them all with a single function call. This makes your input system more maintainable and easier to extend as your game grows.

**/
