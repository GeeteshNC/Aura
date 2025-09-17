// Copy right GeetesNC

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;
UENUM(BlueprintType)
enum class EEffectApplicationPolicy :uint8
{

AppyOnOverlap,
ApplyOnEndOverlap,
DoNotApply

};

UENUM(BlueprintType)//Applicable to infinite gameplay effects because others remove themselves
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
	bool bDestroyOnEffectRemoval=false;
	

	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
	EEffectApplicationPolicy InstantApplicationPolicy=EEffectApplicationPolicy::DoNotApply;
    
   	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
    TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
    
    UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
    EEffectApplicationPolicy DurationApplicationPolicy= EEffectApplicationPolicy::DoNotApply;
    
    UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
    TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
    
    UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
    EEffectApplicationPolicy InfiniteApplicationPolicy= EEffectApplicationPolicy::DoNotApply;
    
     UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Applied Effects")
    EEffectRemovalPolicy InfiniteRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;



};
