// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ShooterCharacter.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AShooterCharacter>(TryGetPawnOwner());
	
}

void UShooterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (Character == nullptr)
	{
		Character = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if (Character)
	{
		Speed = Character->GetVelocity().Size();
		IsInAir = Character->GetMovementComponent()->IsFalling();
		
		//Direction
		FRotator AimRotation = Character->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity());
		Direction = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		FString RotationMesagge = FString::Printf(TEXT("Direction : %f"), Direction);
		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Red, RotationMesagge);
	
		if (Character->GetEquipWeapon())
		{
			ItemType = Character->GetEquipWeapon()->GetItemType();
		}

		IsCrouch = Character->GetbIsCrouch();
	}
}
