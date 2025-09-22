// Copy right GeetesNC


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Actor/AuraEffectActor.h"

void UAuraAbilitySystemComponent::AbilityCharacterInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::EffectApplied );
		
}


void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
FGameplayTagContainer TagContainer;
EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
	

	
	
}