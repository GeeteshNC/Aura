// Copy right GeetesNC

#include "Player/AuraPlayerController.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Engine/LocalPlayer.h"
#include "Input/AuraInputComponent.h"
#include  "NavigationSystem.h"
#include  "Navigationpath.h"
#include "Interaction/EnemyInterface.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates=true;
	Spline=CreateDefaultSubobject<USplineComponent>("Spline");

		
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	AutoRun();

	
	
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn=GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(),ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline,ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline-CachedDestination).Length();
		if (DistanceToDestination<= AutoRunAcceptanceRadius)
		{
			bAutoRunning=false;
		}

	}
}


void AAuraPlayerController::CursorTrace()
{
	
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	if (ThisActor!=LastActor)
	{
		if (LastActor)LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
	}

	/**
	 * A. Last Actor in null  &&ThisActor is null
	 * -DO Nothing
	 * B. Last Actor in null  &&ThisActor is valid
	 * -HighLight ThisActor
	 * C. Last Actor in valid  &&ThisActor is null
	 * -UnHighlight Last Actor
	 * D. Both actors are valid but LastActor != ThisActor
	 * -UnHighlight Last actor and Highlight This Actor
	 * Both Actors are valid and same actors
	 * -Do Nothing
	 * */

	/**if (LastActor==nullptr)
	{
		if (ThisActor!=nullptr)
		{
			//Case B
			ThisActor->HighlightActor();
		}
		else
		{
			//Case A -both are null Do nothing
			
		}
			
	}
	else //LastActor is valid
	{
		if (ThisActor == nullptr)
		{
			//case C
			LastActor->UnHighlightActor();
			
		}
		else// both actors are valid
			if (LastActor != ThisActor)
				
			{
				//case D
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();

			}
		
			else
			
			{
				//Case E -Do nothing
			}
	}**/
}

	void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
	{
         if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
      {
	     bTargeting=ThisActor? true:false;
	     bAutoRunning=false;
      }
		
	}


	void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityLInputTagReleased(InputTag);
	
		return;
	}

	if (bTargeting)
	{
		if (GetASC()) GetASC()->AbilityLInputTagReleased(InputTag);
		
	}
	else
	{
		/**const APawn* ControlledPawn=GetPawn();
		if (FollowTime<=ShortPressThreshold && ControlledPawn)
			
		{
			if (UNavigationPath* NavPath= UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(),CachedDestination ))
			{
				Spline->ClearSplinePoints();

				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					
				}

				CachedDestination=NavPath->PathPoints[NavPath->PathPoints.Num()-1];

				bAutoRunning=true;
				
				
			} As per Aura Class**/

		const APawn* ControlledPawn=GetPawn();
		if (FollowTime<=ShortPressThreshold && ControlledPawn)
			
		{
			if (UNavigationPath* NavPath= UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(),CachedDestination ))
			{
				// Guard against invalid or empty paths
				if (NavPath->IsValid() && NavPath->PathPoints.Num() > 0)
				{
					Spline->ClearSplinePoints();

					for (const FVector& PointLoc : NavPath->PathPoints)
					{
						Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					}

					// Use Last() to access the final point safely
					CachedDestination = NavPath->PathPoints.Last();

					bAutoRunning = true;
				}
			}

		}
		FollowTime=0.f;
		bTargeting=false;
		
	}
}


    void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
	{

	    if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
			
		}
		return;
		
	
	}
	      if (bTargeting)
	   {
           if (GetASC())
       {
    	GetASC()->AbilityInputTagHeld(InputTag);
    }
		
	}
	else
	{
		FollowTime+=GetWorld()->GetDeltaSeconds();
	
		if (CursorHit.bBlockingHit)
		{
			CachedDestination=CursorHit.ImpactPoint;
		}
		
	
		if (APawn* ControlledPawn=GetPawn())
		{
			const FVector WorldDirection=(CachedDestination-ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
	
	}

	UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
	{

		if (AuraAbilitySystemComponent==nullptr)
		{
			AuraAbilitySystemComponent= Cast<UAuraAbilitySystemComponent> ( UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
		
		}

		return AuraAbilitySystemComponent;
	}



void AAuraPlayerController::BeginPlay()
	{
		Super::BeginPlay();
		check(AuraContext);
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		if(Subsystem)
		{
			Subsystem->AddMappingContext(AuraContext,0);
		}
	
		bShowMouseCursor=true;
		DefaultMouseCursor = EMouseCursor::Default;
		FInputModeGameAndUI InputModeData;
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputModeData.SetHideCursorDuringCapture(false);
		SetInputMode(InputModeData);
	
	}

	void AAuraPlayerController::SetupInputComponent()
	{
		Super::SetupInputComponent();
		UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);//crashes if cast fails
		AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,this,  &AAuraPlayerController::Move);
		AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
	}

	void AAuraPlayerController:: Move(const FInputActionValue& InputActionValue)
	{
		const FVector2D InputAxisVector= InputActionValue.Get<FVector2D>();
		const FRotator Rotation=GetControlRotation();
		const FRotator YawRotation(0.f,Rotation.Yaw,0.f);
		const FVector ForwardDirection=FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);//To get forward vector corresponds to yaw, Unit axis with length unit vector 1.
		const FVector RightDirection=FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); //Get Right Direction

		if (APawn* ControlledPawn = GetPawn<APawn>())
		{
			ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
			ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	
		}
	
	};

