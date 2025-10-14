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
