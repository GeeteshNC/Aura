// Copy right Geetes	 NC

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UCurveTable;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
    Elementalist,
	Warrior,
	Ranger
	
};

USTRUCT(BlueprintType)

struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,Category="ClassDefaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	
};

/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,Category="CharacterClassDefaults")
	TMap<ECharacterClass,FCharacterClassDefaultInfo> CharacterClassInformation;
	
	UPROPERTY(EditDefaultsOnly,Category="ClassCommonDefaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	UPROPERTY(EditDefaultsOnly,Category="ClassCommonDefaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(EditDefaultsOnly,Category="ClassCommonDefaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	UPROPERTY(EditDefaultsOnly,Category="ClassCommonDefaults|Damage")
	TObjectPtr<UCurveTable> DamageCalcuationCoefficients;
	

	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass) ;

	
};


