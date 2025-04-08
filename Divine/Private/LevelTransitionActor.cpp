#include "LevelTransitionActor.h"
#include "Kismet/GameplayStatics.h"
#include "SGameModeBase.h"
#include "GameFramework/Pawn.h"

ALevelTransitionActor::ALevelTransitionActor()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ALevelTransitionActor::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("Level Transition Actor has begun play at location: %s"), *GetActorLocation().ToString());
    UE_LOG(LogTemp, Log, TEXT("Mesh is %s"), MeshComp->IsVisible() ? TEXT("visible") : TEXT("not visible"));
}

void ALevelTransitionActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    UE_LOG(LogTemp, Log, TEXT("Level Transition Actor EndPlay called. Reason: %d"), (int32)EndPlayReason);
}

void ALevelTransitionActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);
    if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
    {
        UE_LOG(LogTemp, Log, TEXT("Transition actor overlapped by player, triggering level change."));
        ASGameModeBase* GM = Cast<ASGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
        if (GM)
        {
            GM->OnLevelChanged();
            Destroy();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Game mode not found!"));
        }
    }
}
