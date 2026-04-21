// Fill out your copyright notice in the Description page of Project Settings.


#include "CollecterActorComponent.h"
#include "CollectableActorComponent.h"
#include "CollectableSubsystem.h"
#include "Components/SphereComponent.h"

// Called when the game starts
void UCollecterActorComponent::BeginPlay()
{
	Super::BeginPlay();

	auto actor = GetOwner();
	
	auto collider = actor->FindComponentByClass<USphereComponent>();
	collider->OnComponentBeginOverlap.AddDynamic(this, &UCollecterActorComponent::OnOverlapBegin);
}

void UCollecterActorComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto comp = OtherActor->FindComponentByClass(UCollectableActorComponent::StaticClass());
	if (IsValid(comp))
	{
		OtherActor->Destroy();

		auto collectable = Cast<UCollectableActorComponent>(comp);
		//bool isValid = IsValid(collectable);
		//if (IsValid(collectable))
		GetWorld()->GetGameInstance()->GetSubsystem<UCollectableSubsystem>()->AddCollectable(collectable->Value);
	}
}