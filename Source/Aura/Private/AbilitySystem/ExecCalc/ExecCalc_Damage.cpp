// Copy right GeetesNC


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);

	AuraDamageStatics()
	{

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Armor,Target,false );
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,ArmorPenetration,Source,false );
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,BlockChance,Target,false );
	}
};


static const AuraDamageStatics DamageStatics()
{
	{
		static AuraDamageStatics DStatics;
		return DStatics;
	
	}

}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	
	
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC= ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC= ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar= SourceASC ? SourceASC->GetAvatarActor(): nullptr;
	AActor* TargetAvatar= TargetASC ? TargetASC->GetAvatarActor(): nullptr;
	ICombatInterface* SourceCombatInterface= Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface= Cast<ICombatInterface>(TargetAvatar);

	const FGameplayEffectSpec& Spec =ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;


    //Get Damage Set By Caller Magnitude
	float Damage=Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);

	//Capture BlockChance on target and determine if there was a successful block

	
	float TargetBlockChance=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef,EvaluationParameters,TargetBlockChance);
	TargetBlockChance=FMath::Max<float>(TargetBlockChance,0.f);

	const bool bBlocked= FMath::RandRange(0.f,100.f)<=TargetBlockChance;

	//IF Block, halve the damage.
     Damage=bBlocked ? Damage/2.f:Damage;

	
	/**if (bBlocked)
	{
		Damage=Damage/=2.f;
	} **/
	float TargetArmor=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef,EvaluationParameters,TargetArmor);
	TargetBlockChance=FMath::Max<float>(TargetArmor,0.f);

	const UCharacterClassInfo* CharacterClassInfo= UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	const FRealCurve* ArmorPenetrationCurve=CharacterClassInfo->DamageCalcuationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient= ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());

	//ArmorPenetration Ignores a percentage of the Target's Armor
	
	float SourceArmorPenetration=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef,EvaluationParameters,SourceArmorPenetration);
	SourceArmorPenetration=FMath::Max<float>(SourceArmorPenetration,0.f);
	
	
	const float EffectiveArmor= TargetArmor*(100-SourceArmorPenetration*ArmorPenetrationCoefficient/100.f);
	
		const FRealCurve* EffectiveArmorCurve=CharacterClassInfo->DamageCalcuationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
		const float EffectiveArmorCoefficient= EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	//Armor ignores a percentage of incoming damage.
     Damage*=(100-EffectiveArmor*EffectiveArmorCoefficient)/100.f;
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(),EGameplayModOp::Additive,Damage);


	OutExecutionOutput.AddOutputModifier(EvaluatedData);
  

	
	
}
