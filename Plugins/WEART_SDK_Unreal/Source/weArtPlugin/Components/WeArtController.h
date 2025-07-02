#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "WeArtMessageSerializer.h"
#include "WeArtThimbleTrackingObject.h"
#include "WeArtThimbleSensorObject.h"
#include "WeArtTrackingCalibration.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "WeArtStatusTracker.h"
#include "WeArtCalibrationUX.h"
#include <string>

#include "WeArtSettings.h"
#include "Containers/Ticker.h"
#include "WeArtController.generated.h"

enum WeArtMessageType
{
	MessageSent, MessageReceived
};

enum WeArtError
{
	ConnectionError, SendMessageError, ReceiveMessageError
};

static char messagesSeparator = '~';

/// <summary>
/// Weart controller, used to connect to the Weart middleware, perform operations and receive messages
/// </summary>
UCLASS()
class WEARTPLUGIN_API UWeArtController : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

	UPROPERTY()
	int32 AlwaysLoadedLevels;
	
public:

	UWeArtController();
	virtual ~UWeArtController() override;
	
	UPROPERTY()
	AWeArtTrackingCalibration* TrackingCalibration;
	UPROPERTY()
	AWeArtCalibrationUX* CalibrationUX;
	UPROPERTY()
	AWeArtStatusTracker* StatusTracker;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void BeginDestroy() override;
	
	UFUNCTION(BlueprintCallable)
	void PauseController();
	UFUNCTION(BlueprintCallable)
	void UnpauseController();
	UFUNCTION(BlueprintCallable)
	void StartCalibration();
	UFUNCTION(BlueprintCallable)
	void StopCalibration();
	UFUNCTION(BlueprintCallable)
	bool GetIsAutoCalibrationOn();
	UFUNCTION(BlueprintCallable)
	bool GetIsControllerAutoStart();
	UFUNCTION(BlueprintCallable)
	bool GetIsAutoRawDataOn();
	UFUNCTION(BlueprintCallable)
	void RequestMiddlewareStatus();
	UFUNCTION(BlueprintCallable)
	void Reset();
	UFUNCTION(BlueprintCallable)
	void RestartConnection();
	UFUNCTION(BlueprintCallable)
	void RequestMiddlewareAndDeviceData();
	UFUNCTION(BlueprintCallable)
	void InitBlueprintActors();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetGesturesEnabled();
	void ScheduleController();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetOrderToHideHand();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsRightHandConnected();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsLeftHandConnected();

	void SendMessage(WeArtMessage* message);
	bool ReceiveMessages(std::vector<WeArtMessage*>& messages);

	void OnMessageReceived(WeArtMessage* msg);

	UFUNCTION(BlueprintCallable)
	void StopConnection();

	void LogMessage(WeArtMessageType type, const FString& message);
	void LogError(WeArtError error, FString& message) const;

	void HandleReceivedMessage(const FString& message);
	// We are keeping track of attached thimble tracking objects,
	// so we can forward received messages to them. Usually
	// that could be solved via delegates, but native C++
	// has no support for those, and UE4 delegates would require
	// the WeArtMessage object to be an UCLASS for some reason.
	UPROPERTY()
	TArray<UWeArtThimbleTrackingObject*> thimbleTrackingObjects;
	UPROPERTY()
	TArray<UWeArtThimbleSensorObject*> thimbleSensorObjects;

	UPROPERTY(BlueprintReadOnly)
	EDeviceGeneration DeviceGeneration = EDeviceGeneration::TouchDiver;
	
protected:
	
	UPROPERTY()
	int32 ClientPort;

	UPROPERTY()
	bool DebugMessages;

	UPROPERTY()
	bool ControllerAutoStart;

	UPROPERTY()
	bool CalibrationAutoStart;

	UPROPERTY()
	bool RawDataAutoStart;
	
	UPROPERTY()
	bool GesturesEnabled;

	bool foundTrackingCalibration = false;
	bool foundCalibrationUX = false;
	bool foundStatusTracker = false;
	bool isCancellationRequested = false;
	
	FSocket* socket;
	WeArtMessageSerializer messageSerializer;
	uint8* messageReceivedBuffer[1024];
	std::string trailingText;
	bool IsConnected = false;
	bool IsPaused = false;

	FTimerHandle TimerHandle;
	float addrCreateInterval = 1.f;

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;
	virtual UWorld* GetWorld() const override;

private:
	bool DebugTrackingData;
	FString addrStr = "";
	TSharedPtr<FInternetAddr> addr;
	TAtomic<bool> StopInitBlueprintActors = false;
	bool AreValidFunctorResources(const AActor* Actor) const;
	void LoadConfigValues();
	UFUNCTION()
	void OnLevelLoaded();
	bool isRightHandConnected = false;
	bool isLeftHandConnected = false;
	bool timeToHideNotUsedHand = false;
};
