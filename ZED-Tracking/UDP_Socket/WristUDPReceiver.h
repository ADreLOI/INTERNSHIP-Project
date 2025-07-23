#pragma once

// Ricezione e parsing di pacchetti UDP contenenti i dati dei polsi.

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sockets.h"
#include "Networking.h"
#include "Json.h"
#include "WristUDPReceiver.generated.h"

// Struttura che rappresenta posizione e rotazione di un singolo polso
USTRUCT(BlueprintType)
struct FWristData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Wrist")
    FVector Position = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category="Wrist")
    FRotator Rotation = FRotator::ZeroRotator;
};

// Attore responsabile della ricezione dei dati e del loro esporre a Blueprint
UCLASS()
class AWristUDPReceiver : public AActor
{
    GENERATED_BODY()

public:
    // Costruttore
    AWristUDPReceiver();

    // Ciclo di vita dell'attore Unreal
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(BlueprintReadOnly, Category="Wrist")
    FWristData LeftWrist;

    UPROPERTY(BlueprintReadOnly, Category="Wrist")
    FWristData RightWrist;

private:
    // Decodifica la stringa JSON ricevuta popolando le strutture di output
    bool ParseJson(const FString& JsonString);
    // Inizializza il socket UDP in ascolto
    void SetupSocket();
    // Chiude e distrugge il socket
    void CloseSocket();

    // Puntatore al socket UDP
    FSocket* Socket;
    // Porta su cui restare in ascolto
    int32 Port = 5005;
    // Indirizzo remoto del mittente (opzionale)
    TSharedPtr<FInternetAddr> RemoteAddr;
};
