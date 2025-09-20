// Copy right GeetesNC


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystem/AuraAttributeSet.h"

void UAuraAbilitySystemComponent::AbilityCharacterInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::EffectApplied );
		
}


void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)

{

	GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Effect Applied"));
		
}
