// Copy right GeetesNC

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraProjectileSpell.generated.h"


class AAuraProjectile;
class UGameplayEffect;


UCLASS()
class AURA_API UAuraProjectileSpell : public UAuraGameplayAbility
{
	GENERATED_BODY()

	protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable,Category="Projectile")
		void SpawnProjectile( const FVector& ProjectileTargetLocation);
	

	UPROPERTY(EditAnywhere,BluePrintReadOnly)
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly,BluePrintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
};

/**
 *# 🎯 Aura Projectile System Documentation
## 📖 Table of Contents
1. [Overview](#overview)
2. [What is the Aura Projectile System?](#what-is-it)
3. [How It Works](#how-it-works)
4. [Component Breakdown](#component-breakdown)
5. [Creating Your Own Projectile Spell](#creating-your-own)

## Overview
The **Aura Projectile System** is a framework for creating spell-based projectiles in Unreal Engine. Think of it like a magical fireball or ice bolt that a character can cast at enemies. This system handles everything from spawning the projectile to dealing damage when it hits something.
## What is the Aura Projectile System? 🤔
The system consists of two main classes working together:
### 1. **UAuraProjectileSpell** (The Ability)
- **What**: This is the "spell casting" part - it decides _when_ and _where_ to create a projectile
- **Why**: Separates the casting logic from the projectile itself, making it reusable for different spells
- **Real-world analogy**: Think of this as the "spell book" that tells you how to cast the spell

### 2. **AAuraProjectile** (The Projectile)
- **What**: This is the actual "magic missile" flying through the air
- **Why**: Handles movement, collision detection, visual effects, and damage application
- **Real-world analogy**: This is the actual fireball you see flying toward the target

## How It Works 🔄
### The Complete Flow:
``` 
Player Casts Spell → UAuraProjectileSpell Activates → Spawns AAuraProjectile 
→ Projectile Flies → Hits Target → Deals Damage → Destroys Itself
```
### Step-by-Step Process:
#### **Step 1: Ability Activation**
``` cpp
virtual void ActivateAbility(...)
```
- **When**: Triggered when the player uses the ability (e.g., presses a button)
- **What happens**: The game calls this function to start the spell-casting process
- **Why virtual**: Allows you to customize how different spells activate

#### **Step 2: Spawning the Projectile**
``` cpp
void SpawnProjectile(const FVector& ProjectileTargetLocation)
```
- **When**: Called during ability activation
- **What it needs**: A target location (where you're aiming)
- **What it does**:
    - Creates a new projectile actor in the world
    - Sets its starting position (usually from the caster's hand or staff)
    - Configures the damage it will deal
    - Aims it at the target location

#### **Step 3: Projectile Lifetime**
Once spawned, the projectile:
- **Moves** automatically using `UProjectileMovementComponent` (handles physics)
- **Checks for collisions** using `USphereComponent` (invisible sphere around the projectile)
- **Plays effects** like particle effects and sounds while flying

#### **Step 4: Collision Detection**
``` cpp
void OnSphereOverlap(...)
```
- **When**: Called automatically when the projectile touches something
- **What it does**:
    - Determines what was hit (enemy, wall, etc.)
    - Applies damage if it's a valid target
    - Plays impact effects (explosion, sound)
    - Destroys the projectile

## Component Breakdown 🔧
### UAuraProjectileSpell Properties:
#### `ProjectileClass`
``` cpp
TSubclassOf<AAuraProjectile> ProjectileClass;
```
- **What**: Blueprint reference to what projectile to spawn
- **Why**: Allows designers to choose different projectile types (fireball, ice bolt, etc.) without coding
- **How to use**: Set this in the Blueprint editor to your projectile Blueprint
- **Exposed in**: Blueprint (EditAnywhere, BlueprintReadOnly)

#### `DamageEffectClass`
``` cpp
TSubclassOf<UGameplayEffect> DamageEffectClass;
```
- **What**: Defines how much damage and what type
- **Why**: Uses Unreal's Gameplay Ability System for flexible damage calculations
- **How to use**: Create a Gameplay Effect Blueprint that defines damage amount, type, etc.
- **Exposed in**: Blueprint (EditDefaultsOnly, BlueprintReadOnly)

### AAuraProjectile Properties:
#### `ProjectileMovement`
``` cpp
TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
```
- **What**: Controls how the projectile flies (speed, gravity, homing, etc.)
- **Why**: Built-in Unreal component that handles complex physics for you
- **Visible in editor**: Yes (VisibleAnywhere)

#### `DamageEffectSpecHandle`
``` cpp
FGameplayEffectSpecHandle DamageEffectSpecHandle;
```
- **What**: The actual damage instance that will be applied on hit
- **Why**: Created from the DamageEffectClass with specific values
- **Special**: ExposeOnSpawn means it's set when the projectile is created
- **Access**: Blueprint Read/Write

#### `Sphere`
``` cpp
TObjectPtr<USphereComponent> Sphere;
```
- **What**: Invisible collision sphere
- **Why**: Detects when the projectile hits something
- **Size**: Configurable in Blueprint

#### Visual/Audio Effects:
``` cpp
TObjectPtr<UNiagaraSystem> ImpactEffect;    // Particle effect on hit
TObjectPtr<USoundBase> ImpactSound;         // Sound on hit
TObjectPtr<USoundBase> LoopingSound;        // Sound while flying
```
- **What**: Make the spell look and sound good
- **Why**: Feedback for the player
- **Configurable**: Yes, set in Blueprint editor

#### `LifeSpan`
``` cpp
float LifeSpan = 15.f;
```
- **What**: How long (seconds) before the projectile auto-destroys
- **Why**: Prevents projectiles from existing forever if they miss

## Creating Your Own Projectile Spell 🎨
### For Beginners (Blueprint Only):
1. **Create a Projectile Blueprint**
    - Right-click in Content Browser → Blueprint Class → Search for `AuraProjectile`
    - Set visual effects (Niagara systems for particles)
    - Set sounds
    - Adjust sphere collision size
    - Configure ProjectileMovement (speed, homing, etc.)

2. **Create a Damage Effect**
    - Right-click → Gameplay → Gameplay Effect
    - Set damage amount
    - Set damage type (fire, ice, etc.)

3. **Create Your Spell Ability**
    - Right-click → Blueprint Class → Search for `AuraProjectileSpell`
    - Set `ProjectileClass` to your projectile Blueprint
    - Set `DamageEffectClass` to your damage effect

4. **Grant the Ability**
    - Add the ability to your character's Ability System Component

### Key Customization Points:

| Property | Location | What You Can Change |
| --- | --- | --- |
| **Speed** | Projectile BP → ProjectileMovement | How fast it flies |
| **Damage** | Damage Effect | How much it hurts |
| **Size** | Projectile BP → Sphere | Collision detection radius |
| **Visuals** | Projectile BP → ImpactEffect | Particle effects |
| **Lifetime** | Projectile BP → LifeSpan | How long before it disappears |
## Common Questions ❓
**Q: Why two classes?**
A: Separating the ability (spell casting) from the projectile (thing that flies) makes the code reusable. You can have 10 different spells that all use the same base projectile but with different effects.
**Q: What is BlueprintCallable?**
A: It means you can call this function from Blueprint visual scripting, not just C++ code.
**Q: What is TSubclassOf?**
A: A safe way to reference a class type. It ensures you can only assign Projectile classes to ProjectileClass, preventing errors.
**Q: Why is DamageEffectSpecHandle exposed on spawn?**
A: Because each projectile needs its own unique damage instance. The spell creates it when spawning the projectile.
## Summary 📝
The Aura Projectile System provides a clean, modular way to create spell projectiles:
- **UAuraProjectileSpell**: Handles spell activation and spawning
- **AAuraProjectile**: The flying projectile that deals damage

This separation makes it easy to create many different spells with minimal code duplication, and most customization can be done entirely in Blueprint!
**/
