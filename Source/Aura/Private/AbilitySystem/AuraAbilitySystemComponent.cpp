// Copy right GeetesNC


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Engine/Engine.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Actor/AuraEffectActor.h"

void UAuraAbilitySystemComponent::AbilityCharacterInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::ClientEffectApplied );

	
}


void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpAbilities)
{

	for (const TSubclassOf<UGameplayAbility>AbilityClass:StartUpAbilities)

	{
		FGameplayAbilitySpec AbilitySpec=FGameplayAbilitySpec(AbilityClass,1);

		if (const UAuraGameplayAbility* AuraAbility= Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			GiveAbility(AbilitySpec);
			
		}	
	
		
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())return;
	for (FGameplayAbilitySpec& AbilitySpec:GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
				
			}
		}
	}
	
}

void UAuraAbilitySystemComponent::AbilityLInputTagReleased(const FGameplayTag& InputTag)
{

	if (!InputTag.IsValid())return;
	for (FGameplayAbilitySpec& AbilitySpec:GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
			
		}
	}
		
	}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,//check
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}

