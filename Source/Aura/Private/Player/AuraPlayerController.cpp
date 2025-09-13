// Copy right GeetesNC

#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates=true;
	
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

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

	if (LastActor==nullptr)
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
	}
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
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);//crashes if cast fails
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,this,  &AAuraPlayerController::Move);
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
	
}

