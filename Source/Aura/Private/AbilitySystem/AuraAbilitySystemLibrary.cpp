// Copy right GeetesNC


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
		
	{

		if (AAuraHUD* AuraHUD= Cast<AAuraHUD>( PC->GetHUD()))


		{


			AAuraPlayerState* PS =PC->GetPlayerState<AAuraPlayerState>();

			UAbilitySystemComponent* ASC= PS->GetAbilitySystemComponent();
			UAttributeSet* AS= PS->GetAttributeSet();

			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
			
			
		}
	}
return nullptr;
	
	
	
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
		
	{

		if (AAuraHUD* AuraHUD= Cast<AAuraHUD>( PC->GetHUD()))


		{


			AAuraPlayerState* PS =PC->GetPlayerState<AAuraPlayerState>();

			UAbilitySystemComponent* ASC= PS->GetAbilitySystemComponent();
			UAttributeSet* AS= PS->GetAttributeSet();

			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
			
			
		}
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	
	AActor* AvatarActor=ASC->GetAvatarActor();

	UCharacterClassInfo* CharacterClassInfo= GetCharacterClassInfo(WorldContextObject);
	FGameplayEffectContextHandle PrimaryAttributesContextHandle= ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	FCharacterClassDefaultInfo ClassDefaultInfo=CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	

	const FGameplayEffectSpecHandle PrimaryAttributeSpecHandle=ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);///** FGameplayEffectSpecHandle Allows blueprints to generate a GameplayEffectSpec once and then reference it by handle, to apply it multiple times/multiple targets. */
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributeSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle= ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	

	const FGameplayEffectSpecHandle SecondaryAttributeSpecHandle=ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level,SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributeSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle= ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	

	const FGameplayEffectSpecHandle VitalAttributeSpecHandle=ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributeSpecHandle.Data.Get());

	
	
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	UCharacterClassInfo* CharacterClassInfo= GetCharacterClassInfo(WorldContextObject);
	for (TSubclassOf<UGameplayAbility> Abilityclass: CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec=FGameplayAbilitySpec(Abilityclass,1);
		ASC->GiveAbility(AbilitySpec);
	}
	
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{

	AAuraGameModeBase* AuraGameMode=Cast <AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode==nullptr) return nullptr;

	return AuraGameMode->CharacterClassInfo;
}

/**🎯 What is GameplayEffectSpec?
In Unreal Engine's GAS, a GameplayEffectSpec is a runtime instance of a UGameplayEffect class. Think of it as the configured blueprint of an effect that’s ready to be applied to one or more targets.

UGameplayEffect: This is the static asset — like a class definition.

GameplayEffectSpec: This is the dynamic instance — like an object created from that class, with specific parameters (level, context, tags, etc.).

🧠 Purpose of GameplayEffectSpec
Its main job is to encapsulate all the data needed to apply an effect — including:

The effect class (e.g. PrimaryAttributes)

The level of the effect (which can scale magnitude)

The effect context (who caused it, what tags are involved, etc.)

Any modifiers, tags, or custom data added at runtime

This makes it extremely powerful and flexible for:

Applying effects to multiple targets

Reusing the same spec across different actors

Supporting prediction and replication in multiplayer

Dynamically adjusting effects based on gameplay conditions

🧪 Your Code Breakdown
cpp
FGameplayEffectSpecHandle PrimaryAttributeSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, ASC->MakeEffectContext());
ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributeSpecHandle.Data.Get());
MakeOutgoingSpec(...): Creates a GameplayEffectSpecHandle, which wraps a GameplayEffectSpec instance.

ApplyGameplayEffectSpecToSelf(...): Applies the spec to the owner of the ASC (Ability System Component).

This means you're applying a customized, runtime-configured effect to the player or actor, using attributes defined in PrimaryAttributes.

🧩 Why Use Spec Handles?
Blueprint-friendly: You can create and reuse specs in Blueprints.

Efficiency: Avoids re-instantiating effects repeatedly.

Multiplayer-safe: Supports replication and prediction.**/

/**what is the differrence between . operator and -> operator when we access something, when to use which one
Great question, NC — and it's one that every C++ dev should master early on. Here's a clear breakdown:

🧩 Dot (.) vs Arrow (->) Operator in C++
Both are used to access members (variables or functions) of a class, struct, or union — but they differ based on how you're referencing the object.

Operator	Used With	Meaning	Example
. (Dot)	Object or reference	Access member directly	myObject.health
-> (Arrow)	Pointer to object	Dereference pointer, then access member	myObjectPtr->health
🔍 When to Use Each
Use . when you have a direct object or a reference:

cpp
Player player;
player.health = 100;
Use -> when you have a pointer to an object:

cpp
Player* playerPtr = new Player();
playerPtr->health = 100;
Internally, ptr->member is shorthand for (*ptr).member.

🧠 Quick Tip
If you're ever unsure which to use:

Ask: “Is this a pointer?” → Use ->

Otherwise → Use .

For Unreal Engine devs like you, this distinction is crucial when working with UObjects, Actors, and Components, especially since many UE functions return pointers (GetOwner(), GetWorld(), etc.).

You can find a deeper explanation on GeeksforGeeks and TutorialsPoint.

Would you like a printable cheat sheet that covers these and other C++ syntax essentials for Unreal workflows?**/
