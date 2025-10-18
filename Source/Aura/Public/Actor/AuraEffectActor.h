// Copy right GeetesNC

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"


class UGameplayEffect;
class UAbilitySystemComponent;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
    ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	RemoveOnEndOverlap,
	DoNotRemove
};


UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	

	AAuraEffectActor();
	
protected:

	virtual void BeginPlay() override;

	
	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
    bool bDestroyEffectRemoval=false;
	
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy=EEffectApplicationPolicy::DoNotApply;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy=EEffectApplicationPolicy::DoNotApply;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy=EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy=EEffectRemovalPolicy::RemoveOnEndOverlap;


	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
	float ActorLevel=1.f;	
	
};
/**
 *# Aura Effect Actor - Beginner Friendly Documentation
## Table of Contents
1. [What is the Aura Effect Actor?](#what-is-it)
2. [Why Do We Use It?](#why-use-it)
3. [Core Concepts](#core-concepts)
4. [How It Works](#how-it-works)
5. [Implementation Guide](#implementation-guide)
6. [Common Use Cases](#common-use-cases)

## What is the Aura Effect Actor? {#what-is-it}
The **Aura Effect Actor** is a reusable C++ Actor class in Unreal Engine that serves as a bridge between **physical game objects** (like health potions, fire traps, or power-up zones) and the **Gameplay Ability System (GAS)**.
In simple terms: it's an actor you can place in your level that automatically applies gameplay effects (like healing, damage, buffs, or debuffs) to characters when they interact with it.
## Why Do We Use It? {#why-use-it}
### The Problem It Solves
Without the Aura Effect Actor, you would need to:
- Write repetitive code for every pickup, trap, or buff zone
- Manually handle overlap events for each object
- Duplicate logic for applying/removing effects
- Hard-code gameplay effect applications everywhere

### The Solution
The Aura Effect Actor provides:
✅ **Reusability**: Create one C++ class, use it for hundreds of different effects
✅ **Flexibility**: Configure behavior entirely in Blueprint without touching code
✅ **Designer-Friendly**: Artists and designers can create new pickups/traps without programming
✅ **GAS Integration**: Properly works with Unreal's Gameplay Ability System
✅ **Automatic Management**: Handles effect application, removal, and cleanup automatically
### Real-World Examples
- **Health Potion**: Instantly heals the player on overlap
- **Fire Trap**: Applies damage over time while standing in it
- **Speed Buff Zone**: Increases movement speed infinitely while inside, removes when leaving
- **Poison Cloud**: Applies a 5-second poison debuff on touch

## Core Concepts {#core-concepts}
### 1. Gameplay Effects (GAS)
The Aura Effect Actor uses Unreal's **Gameplay Ability System**. There are three types of effects:

| Effect Type | Description | Example Use Case |
| --- | --- | --- |
| **Instant** | Applied once, immediately | Health potion, instant damage |
| **Duration** | Lasts for a specific time | 10-second speed buff, 5-second poison |
| **Infinite** | Lasts forever (until manually removed) | Aura that persists while in zone |
### 2. Application Policies
Controls **when** effects are applied:
``` cpp
enum class EEffectApplicationPolicy : uint8
{
    ApplyOnOverlap,      // Apply when actor enters trigger
    ApplyOnEndOverlap,   // Apply when actor leaves trigger
    DoNotApply           // Don't apply this effect type
};
```
**Why this matters**: Different gameplay scenarios need different triggers. A health potion applies on touch, but a "leaving safe zone" debuff applies when you exit.
### 3. Removal Policies
Controls **when** infinite effects are removed:
``` cpp
enum class EEffectRemovalPolicy : uint8
{
    RemoveOnEndOverlap,  // Remove effect when leaving trigger
    DoNotRemove          // Effect persists even after leaving
};
```
**Why this matters**: A temporary speed zone removes the buff when you leave, but a permanent upgrade never removes it.
## How It Works {#how-it-works}
### Workflow Diagram
``` 
Player Overlaps Actor
        ↓
OnOverlap() Called
        ↓
Check Application Policies
        ↓
┌───────┴───────┬─────────────────┐
│               │                 │
Instant?    Duration?        Infinite?
Apply Now   Apply & Track   Apply & Store Handle
                            (for removal later)
                                 ↓
                          Player Leaves
                                 ↓
                          OnEndOverlap() Called
                                 ↓
                          Check Removal Policy
                                 ↓
                          Remove Infinite Effects
```
### Key Functions Explained
#### 1. _OnOverlap(AActor TargetActor)_*
``` cpp
UFUNCTION(BlueprintCallable)
void OnOverlap(AActor* TargetActor);
```
**What it does**: Called when an actor (like the player) enters the trigger volume.
**Behind the scenes**:
1. Checks if Instant effect should apply → Applies it
2. Checks if Duration effect should apply → Applies it
3. Checks if Infinite effect should apply → Applies and stores its handle

#### 2. _OnEndOverlap(AActor TargetActor)_*
``` cpp
UFUNCTION(BlueprintCallable)
void OnEndOverlap(AActor* TargetActor);
```
**What it does**: Called when an actor leaves the trigger volume.
**Behind the scenes**:
1. Checks removal policy for Infinite effects
2. Removes tracked effects if policy allows
3. Optionally destroys the actor itself (`bDestroyEffectRemoval`)

#### 3. _ApplyEffectToTarget(AActor TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)_*
``` cpp
UFUNCTION(BlueprintCallable)
void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);
```
**What it does**: The core logic that actually applies a gameplay effect.
**Behind the scenes**:
1. Gets the target's Ability System Component (ASC)
2. Creates an effect specification with the configured level
3. Applies the effect to the ASC
4. Stores the handle for Infinite effects (so we can remove them later)

## Implementation Guide {#implementation-guide}
### Step 1: Create a Blueprint Based on AuraEffectActor
1. In Unreal Editor, right-click in Content Browser
2. Select **Blueprint Class**
3. Search for and select **AuraEffectActor**
4. Name it (e.g., `BP_HealthPotion`)

### Step 2: Add Visual Components (Blueprint)
In your Blueprint:
1. Add a **Static Mesh** or **Skeletal Mesh** for visuals
2. Add a **Sphere Collision** or **Box Collision** component
3. Set collision to **Overlap** events enabled

### Step 3: Wire Up Overlap Events (Blueprint)
In the Blueprint Event Graph:
``` 
Event ActorBeginOverlap
    ↓
Call: OnOverlap(Other Actor)

Event ActorEndOverlap
    ↓
Call: OnEndOverlap(Other Actor)
```
### Step 4: Configure Gameplay Effects
In the Blueprint details panel, you'll see these categories:
#### Example: Health Potion Configuration

| Property | Value | Explanation |
| --- | --- | --- |
| **InstantGameplayEffectClass** | `GE_HealInstant` | The effect that heals |
| **InstantEffectApplicationPolicy** | `ApplyOnOverlap` | Heal when touched |
| **DurationEffectApplicationPolicy** | `DoNotApply` | No duration effect |
| **InfiniteEffectApplicationPolicy** | `DoNotApply` | No infinite effect |
| **bDestroyEffectRemoval** | `true` | Destroy potion after use |
| **ActorLevel** | `1.0` | Level for effect scaling |
#### Example: Fire Trap Configuration

| Property | Value | Explanation |
| --- | --- | --- |
| **InfiniteGameplayEffectClass** | `GE_FireDamage` | Continuous fire damage |
| **InfiniteEffectApplicationPolicy** | `ApplyOnOverlap` | Start damage on enter |
| **InfiniteEffectRemovalPolicy** | `RemoveOnEndOverlap` | Stop damage on exit |
| **bDestroyEffectRemoval** | `false` | Trap stays in world |
| **ActorLevel** | `1.0` | Damage scaling level |
### Step 5: Create Your Gameplay Effects
You need to create the actual **Gameplay Effect** assets:
1. Right-click in Content Browser → **Gameplay** → **Gameplay Effect**
2. Configure the effect (modify attributes, duration, etc.)
3. Reference it in your AuraEffectActor Blueprint

## Common Use Cases {#common-use-cases}
### 1. **Consumable Health Potion**
- **Instant** effect on overlap
- Destroys after use
- No duration or infinite effects

### 2. **Environmental Hazard (Poison Gas)**
- **Duration** effect on overlap (lasts 10 seconds after leaving)
- Doesn't destroy
- Reusable trap

### 3. **Buff Zone (Speed Boost Area)**
- **Infinite** effect on overlap
- Removes on end overlap
- Doesn't destroy
- Players can enter/exit multiple times

### 4. **Permanent Upgrade Pickup**
- **Infinite** effect on overlap
- Does NOT remove on end overlap
- Destroys after pickup
- Permanent stat boost

### 5. **Damage on Exit (Leaving Safe Zone)**
- **Instant** effect on END overlap
- Punishes leaving an area
- Doesn't destroy

## Important Properties Reference
### Effect Configuration

| Property | Type | Purpose |
| --- | --- | --- |
| `InstantGameplayEffectClass` | Class Reference | Which effect to apply instantly |
| `DurationGameplayEffectClass` | Class Reference | Which effect to apply with duration |
| `InfiniteGameplayEffectClass` | Class Reference | Which effect to apply infinitely |
### Policy Configuration

| Property | Type | Purpose |
| --- | --- | --- |
| `InstantEffectApplicationPolicy` | Enum | When to apply instant effect |
| `DurationEffectApplicationPolicy` | Enum | When to apply duration effect |
| `InfiniteEffectApplicationPolicy` | Enum | When to apply infinite effect |
| `InfiniteEffectRemovalPolicy` | Enum | When to remove infinite effect |
### Behavior Configuration

| Property | Type | Purpose |
| --- | --- | --- |
| `bDestroyEffectRemoval` | Boolean | Destroy actor after use? |
| `ActorLevel` | Float | Level for effect scaling (damage/healing amounts) |
### Internal Tracking

| Property | Type | Purpose |
| --- | --- | --- |
| `ActiveEffectHandles` | Map | Tracks active infinite effects for removal |
## Summary
The **Aura Effect Actor** is a powerful, reusable system that:
- ✅ Bridges physical game objects with the Gameplay Ability System
- ✅ Eliminates repetitive code for pickups, traps, and buff zones
- ✅ Provides designer-friendly Blueprint configuration
- ✅ Automatically manages effect application and removal
- ✅ Supports all three GAS effect types (Instant, Duration, Infinite)
- ✅ Enables complex gameplay scenarios with simple configuration

By understanding these concepts, you can create sophisticated gameplay mechanics (health pickups, damage zones, buff areas, etc.) without writing custom code for each one!
**/
