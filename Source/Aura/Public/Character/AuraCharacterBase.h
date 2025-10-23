// Copy right GeetesNC

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility; 


class UAnimMontage;

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}

	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual void Die() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();
	
	
	

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;


	UPROPERTY(EditAnywhere, Category="Combat")
	FName WeaponTipSocketName;

	virtual FVector GetCombatSocketLocation() override;
	
	

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual void InitAbilityActorInfo() {};
	
    UPROPERTY(BlueprintReadOnly,EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;
	
	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect>GamePlayEffectClass, float Level) const;
	virtual void InitializeDefaultAttributes() const;

	void AddCharacterAbilities();

	/*Dissolve Effects*/

	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeLine(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeLine(UMaterialInstanceDynamic* WeaponDynamicMaterialInstance);
	
    UPROPERTY(EditAnywhere, BlueprintReadOnly)  
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)  
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	
	
	
private:
UPROPERTY(EditAnywhere, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere,Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	

};

/**
 *# AuraInputComponent Documentation
## Overview
**AuraInputComponent** is a custom Enhanced Input Component that extends Unreal Engine's `UEnhancedInputComponent` to provide a streamlined way to bind multiple input actions for ability systems. It's specifically designed to work with Gameplay Ability System (GAS) patterns.
## Why We Use AuraInputComponent
### The Problem It Solves
In a typical game with an ability system (like an RPG or MOBA), you might have:
- **Multiple abilities** (10+ different spells/skills)
- Each ability needs to respond to **three input states**: Pressed, Released, and Held
- Each ability is associated with a **Gameplay Tag** for identification

**Without this component**, you would need to manually bind each ability like this:
``` cpp
// Traditional approach - repetitive and error-prone
InputComponent->BindAction(FireballAction, ETriggerEvent::Started, this, &AMyCharacter::OnFireballPressed);
InputComponent->BindAction(FireballAction, ETriggerEvent::Completed, this, &AMyCharacter::OnFireballReleased);
InputComponent->BindAction(FireballAction, ETriggerEvent::Triggered, this, &AMyCharacter::OnFireballHeld);

InputComponent->BindAction(IceBlastAction, ETriggerEvent::Started, this, &AMyCharacter::OnIceBlastPressed);
InputComponent->BindAction(IceBlastAction, ETriggerEvent::Completed, this, &AMyCharacter::OnIceBlastReleased);
InputComponent->BindAction(IceBlastAction, ETriggerEvent::Triggered, this, &AMyCharacter::OnIceBlastHeld);

// ... repeat for every ability
```
**With AuraInputComponent**, all of this is reduced to a single function call:
``` cpp
// Streamlined approach - one call binds all abilities
BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputPressed, 
                   &ThisClass::AbilityInputReleased, &ThisClass::AbilityInputHeld);
```
### Key Benefits
1. **Reduces Boilerplate Code**: Eliminates repetitive binding code
2. **Centralized Configuration**: All input mappings are defined in a Data Asset
3. **Scalable**: Easy to add new abilities without changing controller code
4. **Maintainable**: Changes to input configuration don't require code recompilation
5. **Type-Safe**: Uses templates to ensure correct function signatures

## How It Works
### Architecture Components
The system consists of three main parts:
1. **UAuraInputComponent** (this class) - The component that performs the binding
2. **UAuraInputConfig** (Data Asset) - Stores the configuration of input actions and their associated tags
3. **FAuraInputAction** (Struct) - Pairs an Input Action with a Gameplay Tag

### The Flow
``` 
┌─────────────────────┐
│ UAuraInputConfig    │
│ (Data Asset)        │
│                     │
│ - Fireball + Tag    │
│ - Ice Blast + Tag   │
│ - Lightning + Tag   │
└──────────┬──────────┘
           │
           │ Referenced by
           ▼
┌─────────────────────┐
│ UAuraInputComponent │
│                     │
│ BindAbilityActions()│
└──────────┬──────────┘
           │
           │ Creates bindings
           ▼
┌─────────────────────┐
│ Your Character      │
│ Controller          │
│                     │
│ AbilityInputPressed()│
│ AbilityInputReleased()│
│ AbilityInputHeld()  │
└─────────────────────┘
```
## Implementation Guide
### Step 1: Understanding the Template Function
The `BindAbilityActions` function uses C++ templates to accept any class and function types:
``` cpp
template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, 
                        PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, 
                        HeldFuncType HeldFunc);
```
**Parameters:**
- `InputConfig`: The Data Asset containing all ability input configurations
- `Object`: The object that will handle the callbacks (usually `this`)
- `PressedFunc`: Function called when input starts (button pressed)
- `ReleasedFunc`: Function called when input completes (button released)
- `HeldFunc`: Function called continuously while input is active (button held)

### Step 2: Create the Input Config Data Asset
First, you need a Data Asset that stores your input configurations. This would be defined elsewhere in your project:
``` cpp
// FAuraInputAction struct (typically in AuraInputConfig.h)
USTRUCT(BlueprintType)
struct FAuraInputAction
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    const UInputAction* InputAction = nullptr;

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag InputTag;
};

// UAuraInputConfig class
UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FAuraInputAction> AbilityInputActions;
};
```
**In the Unreal Editor**, create a Data Asset:
1. Right-click in Content Browser → Miscellaneous → Data Asset
2. Select `AuraInputConfig` as the class
3. Name it (e.g., `DA_InputConfig_Hero`)
4. Open it and add your ability actions with their tags

### Step 3: Using AuraInputComponent in Your Controller
In your Player Controller or Character class:
``` cpp
// In your AuraPlayerController.h
protected:
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UAuraInputConfig> InputConfig;

    void AbilityInputPressed(FGameplayTag InputTag);
    void AbilityInputReleased(FGameplayTag InputTag);
    void AbilityInputHeld(FGameplayTag InputTag);
```

``` cpp
// In your AuraPlayerController.cpp
void AAuraPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Cast to our custom input component
    UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

    // Bind all ability actions in one call
    AuraInputComponent->BindAbilityActions(InputConfig, this, 
                                           &ThisClass::AbilityInputPressed,
                                           &ThisClass::AbilityInputReleased,
                                           &ThisClass::AbilityInputHeld);
}

void AAuraPlayerController::AbilityInputPressed(FGameplayTag InputTag)
{
    // Handle ability activation
    // Example: ActivateAbilityByTag(InputTag);
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, 
                                     FString::Printf(TEXT("Pressed: %s"), *InputTag.ToString()));
}

void AAuraPlayerController::AbilityInputReleased(FGameplayTag InputTag)
{
    // Handle ability release
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
                                     FString::Printf(TEXT("Released: %s"), *InputTag.ToString()));
}

void AAuraPlayerController::AbilityInputHeld(FGameplayTag InputTag)
{
    // Handle ability holding (called every frame while held)
    // Example: ContinueChannelingAbility(InputTag);
}
```
### Step 4: Configure Your Project to Use AuraInputComponent
In your Project Settings or GameMode, ensure that `UAuraInputComponent` is used as the Input Component class:
**Option A: In C++ (GameMode or PlayerController constructor)**
``` cpp
PlayerInputClass = UAuraInputComponent::StaticClass();
```
**Option B: In Blueprint**
- Open your Player Controller Blueprint
- In Class Settings → Input → Input Component Class
- Select `AuraInputComponent`

## Code Breakdown
Let's examine the key parts of the implementation:
### Safety Check
``` cpp
check(InputConfig)
```
- Ensures the InputConfig is valid
- In Development builds, this will crash if null (helps catch errors early)
- In Shipping builds, this is compiled out for performance

### Iteration Over Actions
``` cpp
for (const FAuraInputAction& Action : InputConfig->AbilityInputActions)
```
- Loops through every ability input action defined in the Data Asset
- Uses a const reference for efficiency

### Validation
``` cpp
if (Action.InputAction && Action.InputTag.IsValid())
```
- Checks that both the Input Action and Gameplay Tag are valid
- Skips invalid entries gracefully

### Conditional Binding
``` cpp
if (PressedFunc)
{
    BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
}
```
- Only binds if the function pointer is provided (not null)
- Uses `ETriggerEvent::Started` for initial button press
- Passes the `InputTag` as a parameter to the callback function

**Trigger Event Types:**
- `Started`: Fires once when input begins
- `Completed`: Fires once when input ends
- `Triggered`: Fires every frame while input is active

## Usage Example: Complete Workflow
### 1. Define Your Gameplay Tags
In your `GameplayTags` configuration:
``` 
Input.Ability.Fireball
Input.Ability.IceBlast
Input.Ability.Lightning
```
### 2. Create Input Actions (Enhanced Input)
In the Unreal Editor:
- Create Input Actions: `IA_Fireball`, `IA_IceBlast`, `IA_Lightning`
- Create an Input Mapping Context and map keys to these actions

### 3. Populate the Data Asset
In `DA_InputConfig_Hero`:
``` 
AbilityInputActions[0]:
    InputAction: IA_Fireball
    InputTag: Input.Ability.Fireball

AbilityInputActions[1]:
    InputAction: IA_IceBlast
    InputTag: Input.Ability.IceBlast

AbilityInputActions[2]:
    InputAction: IA_Lightning
    InputTag: Input.Ability.Lightning
```
### 4. Handle the Callbacks
``` cpp
void AAuraPlayerController::AbilityInputPressed(FGameplayTag InputTag)
{
    if (InputTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Input.Ability.Fireball")))
    {
        // Cast Fireball
    }
    else if (InputTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Input.Ability.IceBlast")))
    {
        // Cast Ice Blast
    }
    // Or better: use the tag directly with your Ability System Component
    // AbilitySystemComponent->AbilityInputPressed(InputTag);
}
```
## Best Practices
1. **Use with Gameplay Ability System**: This component is designed to work seamlessly with GAS's `TryActivateAbilitiesByTag` function
2. **Null Function Pointers Are OK**: You can pass `nullptr` for any callback you don't need:
``` cpp
   BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputPressed, nullptr, nullptr);
```
1. **Keep Data Asset Referenced**: Store the `InputConfig` as a `UPROPERTY` to prevent garbage collection
2. **Use ThisClass**: In callback bindings, prefer `&ThisClass::FunctionName` over explicitly typing the class name
3. **Validate in Editor**: Double-check your Data Asset has all actions properly configured before testing

## Common Issues and Solutions
### Issue: Bindings Not Working
**Solution**:
- Verify `UAuraInputComponent` is set as your Input Component Class
- Ensure the Input Mapping Context is added to the player
- Check that InputConfig is assigned in the editor

### Issue: InputTag Is Always Invalid
**Solution**:
- Make sure Gameplay Tags are defined in your project settings
- Verify tags are assigned in the Data Asset
- Check for typos in tag names

### Issue: Held Function Fires Too Often
**Solution**:
- This is expected behavior; `ETriggerEvent::Triggered` fires every frame
- Add throttling logic in your callback if needed
- Consider using a timer instead for periodic effects

## Summary
**AuraInputComponent** simplifies input binding for ability systems by:
- Automatically binding multiple input actions from a Data Asset
- Supporting three input states (Pressed, Released, Held) per action
- Passing Gameplay Tags to callbacks for easy ability identification
- Reducing boilerplate code and improving maintainability

This pattern is especially powerful when combined with Unreal's Gameplay Ability System, creating a clean, scalable input architecture for complex games.
**/