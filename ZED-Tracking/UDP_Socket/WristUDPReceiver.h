#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sockets.h"
#include "Networking.h"
#include "Json.h"
#include "WristUDPReceiver.generated.h"

USTRUCT(BlueprintType)
struct FWristData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Wrist")
    FVector Position = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category="Wrist")
    FRotator Rotation = FRotator::ZeroRotator;
};

UCLASS()
class AWristUDPReceiver : public AActor
{
    GENERATED_BODY()

public:
    AWristUDPReceiver();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(BlueprintReadOnly, Category="Wrist")
    FWristData LeftWrist;

    UPROPERTY(BlueprintReadOnly, Category="Wrist")
    FWristData RightWrist;

private:
    bool ParseJson(const FString& JsonString);
    void SetupSocket();
    void CloseSocket();

    FSocket* Socket;
    int32 Port = 5005;
    TSharedPtr<FInternetAddr> RemoteAddr;
    
};