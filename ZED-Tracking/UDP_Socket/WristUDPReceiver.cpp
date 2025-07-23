#include "WristUDPReceiver.h"

// Implementazione delle funzionalità di ricezione UDP dei dati dei polsi
#include "SocketSubsystem.h"
#include "Common/UdpSocketBuilder.h"
#include "HAL/RunnableThread.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

AWristUDPReceiver::AWristUDPReceiver()
{
    // Abilitiamo il tick per poter controllare costantemente il socket
    PrimaryActorTick.bCanEverTick = true;
    // Il socket verrà creato in BeginPlay
    Socket = nullptr;
}

void AWristUDPReceiver::BeginPlay()
{
    Super::BeginPlay();
    // Apertura del socket UDP alla partenza dell'attore
    SetupSocket();
}

void AWristUDPReceiver::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Se il socket non è disponibile usciamo
    if (!Socket)
    {
        return;
    }

    uint32 Size;
    while (Socket->HasPendingData(Size))
    {
        TArray<uint8> Data;
        Data.SetNumUninitialized(FMath::Min(Size, 65507u));
        int32 Read = 0;
        if (Socket->Recv(Data.GetData(), Data.Num(), Read))
        {
            // Convertiamo il buffer in stringa JSON e lo passiamo al parser
            FString JsonString = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(Data.GetData())));
            ParseJson(JsonString);
        }
    }
}

void AWristUDPReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Pulizia del socket prima che l'attore venga distrutto
    CloseSocket();
    Super::EndPlay(EndPlayReason);
}

bool AWristUDPReceiver::ParseJson(const FString& JsonString)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        // Lambda di utilità per estrarre i dati di un singolo polso
        auto ParseWrist = [&](const FString& Label, FWristData& OutData)
        {
            TSharedPtr<FJsonObject> WristObj = JsonObject->GetObjectField(Label);
            if (!WristObj.IsValid())
            {
                return;
            }
            // Sezione \"position\" con coordinate cartesiane
            TSharedPtr<FJsonObject> PosObj = WristObj->GetObjectField(TEXT(\"position\"));
            if (PosObj.IsValid())
            {
                OutData.Position.X = PosObj->GetNumberField(TEXT(\"x\"));
                OutData.Position.Y = PosObj->GetNumberField(TEXT(\"y\"));
                OutData.Position.Z = PosObj->GetNumberField(TEXT(\"z\"));
            }
            // Sezione \"rotation\" con i tre angoli
            TSharedPtr<FJsonObject> RotObj = WristObj->GetObjectField(TEXT(\"rotation\"));
            if (RotObj.IsValid())
            {
                OutData.Rotation.Pitch = RotObj->GetNumberField(TEXT(\"pitch\"));
                OutData.Rotation.Yaw = RotObj->GetNumberField(TEXT(\"yaw\"));
                OutData.Rotation.Roll = RotObj->GetNumberField(TEXT(\"roll\"));
            }
        };

        // Estraiamo i dati per entrambi i polsi
        ParseWrist(TEXT(\"left_wrist\"), LeftWrist);
        ParseWrist(TEXT(\"right_wrist\"), RightWrist);
        // Parsing avvenuto con successo
        return true;
    }
    // Formato non valido
    return false;
}

void AWristUDPReceiver::SetupSocket()
{
    // Creiamo un endpoint in ascolto su tutte le interfacce
    FIPv4Endpoint Endpoint(FIPv4Address::Any, Port);

    Socket = FUdpSocketBuilder(TEXT(\"WristUDPReceiverSocket\"))
                 .AsNonBlocking()
                 .AsReusable()
                 .BoundToEndpoint(Endpoint)
                 .WithReceiveBufferSize(2 * 1024 * 1024);

    if (Socket)
    {
        UE_LOG(LogTemp, Log, TEXT(\"UDP socket created on port %d\"), Port);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT(\"Failed to create UDP socket\"));
    }
}

void AWristUDPReceiver::CloseSocket()
{
    if (Socket)
    {
        // Chiudiamo e distruggiamo il socket
        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
        Socket = nullptr;
    }
}
