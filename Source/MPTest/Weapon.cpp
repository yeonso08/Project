// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "ShooterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "ShooterHUD.h"

// Sets default values
AWeapon::AWeapon() : 
	ItemType(EItemType::EIT_Pistol), ItemState(EItemState::EIS_Dropped),
	bPressShoot(false), SocketName("Pistol_Socket")
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	
	SetRootComponent(Mesh);
	Box->SetupAttachment(RootComponent);

	Mesh->SetSimulatePhysics(true);
	
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	Box->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::BoxBeginOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &AWeapon::BoxEndOverlap);

}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, ShootMontage);
	DOREPLIFETIME(AWeapon, ShootEffect);
	DOREPLIFETIME(AWeapon, ItemType);
	DOREPLIFETIME(AWeapon, ShootSound);
	DOREPLIFETIME(AWeapon, HitEffect);
	DOREPLIFETIME(AWeapon, HitSound);
}


void AWeapon::BoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		Character->SetOverlappingWeapon(this);
	}
}

void AWeapon::BoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Character == nullptr)
	{
		Character = Cast<AShooterCharacter>(OtherActor);
	}
	if (Character)
	{
		Character->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::PressShoot_Implementation()
{
	if (Character)
	{
		if (ShootMontage)
		{
			Character->PlayAnimMontage(ShootMontage);
		}
		
		const USkeletalMeshSocket* SkeletalSocketName = Mesh->GetSocketByName(FName("MuzzleFlash"));
		if (SkeletalSocketName)
		{
			const FTransform SocketTransform = SkeletalSocketName->GetSocketTransform(Mesh);
			if (ShootEffect && ShootSound)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShootEffect, SocketTransform);
				UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShootSound, SocketTransform.GetLocation());
			}
		}
	}

	LineTrace();
}

void AWeapon::LineTrace()
{
	// LineTrace
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	ViewportSize.X /= 2;
	ViewportSize.Y /= 2;

	FVector WorldPosition;
	FVector WorldDirection;

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == Character->GetController())
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController == Character->GetController()"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FAIL !!! PlayerController != Character->GetController()"));
		return;
	}

	if (Character)
	{
		UGameplayStatics::DeprojectScreenToWorld(
			PlayerController, ViewportSize,
			WorldPosition, WorldDirection);
	}

	FVector Start;
	FVector End;

	const USkeletalMeshSocket* SkeletalSocketName = Mesh->GetSocketByName(FName("MuzzleFlash"));
	if (SkeletalSocketName)
	{
		const FTransform SocketTransform = SkeletalSocketName->GetSocketTransform(Mesh);
		Start = SocketTransform.GetLocation();
	}
	End = WorldPosition + WorldDirection * 50'000.f;

	ReqShoot(Start, End);

	// crosshair
	bIsShoot = true;
	GetWorldTimerManager().SetTimer(IsShootTimer, this, &AWeapon::IsShoot, 0.3f);
}

void AWeapon::IsShoot()
{
	bIsShoot = false;
}

void AWeapon::ReqShoot_Implementation(FVector Start, FVector End)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	bool bResult = GetWorld()->LineTraceSingleByChannel(Hit, Start, End,
		ECollisionChannel::ECC_Camera, Params);

	if (bResult)
	{
		if (Hit.Actor.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit.Actor.IsValid : %s"), *Hit.Actor->GetName());
			FDamageEvent Damage;
			Hit.Actor->TakeDamage(10.f, Damage,
				Character->GetController(), this);
		}

		if (HitEffect && HitSound)
		{
			// Multicast로 보내야 복제됨
			HitEffectSound(Hit.Location);
		}
	}
	

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.f);
	UE_LOG(LogTemp, Warning, TEXT("Hit.BoneName : %s"), *Hit.BoneName.ToString());
}

void AWeapon::PressPickUpItem_Implementation()
{
	if (Character)
	{
		const USkeletalMeshSocket* SkeletalSocketName = Character->GetMesh()->GetSocketByName(SocketName);
		if (SkeletalSocketName)
		{
			if (Character->GetEquipWeapon())
			{
				// 새로 주운 애
				SetItemState(EItemState::EIS_Equipped);
				// 들고 있던 애
				Character->GetEquipWeapon()->SetItemState(EItemState::EIS_NonEquipped);
				//widget

				SkeletalSocketName->AttachActor(this, Character->GetMesh());
				SetOwner(Character);
				Character->SetEquipWeapon(this);
				Character->Inventory.Add(this);
				Character->SetNumberOfWeapon();
				Character->SetCurWeaponNumber(Character->GetNumberOfWeapon());

				
			}
			else
			{
				SetItemState(EItemState::EIS_Equipped);

				SkeletalSocketName->AttachActor(this, Character->GetMesh());
				SetOwner(Character);
				Character->SetEquipWeapon(this);
				Character->Inventory.Add(this);
				Character->SetNumberOfWeapon();
				Character->SetCurWeaponNumber(Character->GetNumberOfWeapon());

			}
		}
	}
}

void AWeapon::SetItemState(EItemState NewItemState)
{
	switch (NewItemState)
	{
	case EItemState::EIS_Dropped:
		ItemState = EItemState::EIS_Dropped;
		Mesh->SetSimulatePhysics(true);
		Mesh->SetVisibility(true);

		Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);

		Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		break;
	case EItemState::EIS_Equipped:
		ItemState = EItemState::EIS_Equipped;
		Mesh->SetSimulatePhysics(false);
		Mesh->SetVisibility(true);

		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);

		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		break;
	case EItemState::EIS_NonEquipped:
		ItemState = EItemState::EIS_NonEquipped;
		Mesh->SetSimulatePhysics(false);
		Mesh->SetVisibility(false);

		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);

		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		break;
	}
}

void AWeapon::HitEffectSound_Implementation(FVector Location)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, Location);
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), HitSound, Location);
}