// Copyright Original Skain Experiences 2019.

#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	ActorThatOpens = PlayerPawn;
}

void  UOpenDoor::OpenDoor() {
	Owner->SetActorRotation(FRotator(0.0f, OpenDoorAngle, 0.0f));
}

void  UOpenDoor::CloseDoor() {
	Owner->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (PressurePlate->IsOverlappingActor(ActorThatOpens)) {
		OpenDoor();
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}
	else {
		if ((LastDoorOpenTime + DoorCloseDelay) < GetWorld()->GetTimeSeconds()) {
			CloseDoor();
		}
	}

}

