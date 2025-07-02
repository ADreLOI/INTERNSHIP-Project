#include "WeArtController.h"
#include "weArtPlugin.h"
#include "Async/Async.h"

#include "Sockets.h"
#include "SocketSubsystem.h"

#include "WeArtSettings.h"
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Engine/LevelStreaming.h"

/**
 * 
 */
UWeArtController::UWeArtController() : AlwaysLoadedLevels(0), TrackingCalibration(nullptr), CalibrationUX(nullptr),
                                       StatusTracker(nullptr),
                                       ClientPort(0),
                                       DebugMessages(false),
                                       ControllerAutoStart(false),
                                       CalibrationAutoStart(false),
                                       RawDataAutoStart(false),
                                       GesturesEnabled(false),
                                       socket(nullptr),
                                       messageReceivedBuffer{}, trailingText(), DebugTrackingData(false)
{
}

UWeArtController::~UWeArtController()
{
	StopConnection();
}

void UWeArtController::LoadConfigValues()
{
	if (UWeArtSettings* Settings = GetMutableDefault<UWeArtSettings>())
	{
		// Use the values read from the .ini file
		ClientPort = Settings->ClientPort;
		DebugMessages = Settings->DebugMessages;
		ControllerAutoStart = Settings->ControllerAutoStart;
		CalibrationAutoStart = Settings->CalibrationAutoStart;
		RawDataAutoStart = Settings->RawDataAutoStart;
		GesturesEnabled = Settings->GesturesEnabled;
		DeviceGeneration = Settings->DeviceGeneration;
	}
}

void UWeArtController::OnLevelLoaded()
{
	if (!--AlwaysLoadedLevels)
	{
		ScheduleController();
	}
}

void UWeArtController::Initialize(FSubsystemCollectionBase& Collection)
{
	LoadConfigValues();
	
	IsPaused = ! ControllerAutoStart;

	TArray<ULevelStreaming*> SteamingLevels = GetWorld()->GetStreamingLevels();
		
	if (!SteamingLevels.Num())
	{
		ScheduleController();
	}
	
	for (ULevelStreaming* streamingLevel : SteamingLevels)
	{
		if (streamingLevel->ShouldBeAlwaysLoaded())
		{
			AlwaysLoadedLevels++;
			streamingLevel->OnLevelLoaded.AddDynamic(this, &UWeArtController::OnLevelLoaded);
		}
	}
}

void UWeArtController::Deinitialize()
{
	// This send does not work, why?
	StopFromClientMessage stopMsg;
	SendMessage(&stopMsg);

	StopConnection();
}

void UWeArtController::BeginDestroy()
{
	Super::BeginDestroy();

	StopInitBlueprintActors = true;
}


void UWeArtController::PauseController()
{
	if (IsPaused) {
		return;
	}

	IsPaused = true;
	Deinitialize();
}


void UWeArtController::UnpauseController()
{
	if (!IsPaused) {
		return;
	}

	IsPaused = false;
	ScheduleController();
}

void UWeArtController::StartCalibration()
{
	StartCalibrationMessage startCalibrationMessage;
	SendMessage(&startCalibrationMessage);
}

void UWeArtController::StopCalibration()
{
	StopCalibrationMessage stopCalibrationMessage;
	SendMessage(&stopCalibrationMessage);

	// CRASH FIXED: added nullref check
	if(foundTrackingCalibration && TrackingCalibration)
	{
		TrackingCalibration->OnMiddlewareCalibrationStop.Broadcast();
	}
	if(foundCalibrationUX && CalibrationUX)
	{
		CalibrationUX->OnUXMiddlewareCalibrationStop.Broadcast();
	}
}

bool UWeArtController::GetIsAutoCalibrationOn()
{
	return CalibrationAutoStart;
}

bool UWeArtController::GetIsControllerAutoStart()
{
	return ControllerAutoStart;
}

bool UWeArtController::GetIsAutoRawDataOn()
{
	return RawDataAutoStart;
}

void UWeArtController::RequestMiddlewareStatus()
{
	GetMiddlewareStatus middlewareStatus;
	SendMessage(&middlewareStatus);
	GetDevicesStatusMessage devicesStatusMessage;
	SendMessage(&devicesStatusMessage);
}

void UWeArtController::Reset()
{
	Deinitialize();
}

void UWeArtController::RestartConnection()
{
	isCancellationRequested = false;
	ScheduleController();
}

void UWeArtController::SendMessage(WeArtMessage* message)
{
	if (!IsConnected) {
		return;
	}

	// WeArt message to string
	std::string text = message->serialize();
	text += messagesSeparator;

	// String to bytestream
	size_t textSize = text.size();
	FString fText(text.c_str());
	TCHAR* bytestream = fText.GetCharArray().GetData();
	int32 size = FCString::Strlen(bytestream);

	// Send the message
	int32 sentAmount = 0;
	if (socket->Send((uint8*)TCHAR_TO_ANSI(bytestream), size, sentAmount)) {
		FString fMessage(text.c_str());
		LogMessage(WeArtMessageType::MessageSent, fMessage);
	}
	else {
		FString fMessage(text.c_str());
		LogError(WeArtError::SendMessageError, fMessage);
	}
}

bool UWeArtController::ReceiveMessages(std::vector<WeArtMessage*>& messages)
{																						
	if (!IsConnected) {
		return false;
	}

	TArray<uint8> ReceivedData;

	uint32 size;
	while (socket->HasPendingData(size))
	{
		// Resize buffer to receive the incoming data chunk
		int32 CurrentBufferSize = ReceivedData.Num();
		ReceivedData.SetNumUninitialized(CurrentBufferSize + FMath::Min(size, 65507u));

		int32 Read = 0;
		socket->Recv(ReceivedData.GetData() + CurrentBufferSize, ReceivedData.Num() - CurrentBufferSize, Read);

		// Adjust buffer size if less data was read than expected
		if (Read < ReceivedData.Num() - CurrentBufferSize) {
			ReceivedData.SetNum(CurrentBufferSize + Read);
		}
	}

	if (ReceivedData.Num() > 0) {
		// Bytestream to one large string
		FString fBufferText = ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData()));

		// Ensure the string is exactly the size of the received data
		if (fBufferText.Len() > ReceivedData.Num()) {
			fBufferText.RemoveAt(ReceivedData.Num(), fBufferText.Len() - ReceivedData.Num());
		}
		
		// For UE-related reasons, the above conversion can leave
		// some garbage entries at the end of the string. Cut them off here.
		if (fBufferText.Len() - ReceivedData.Num() > 0) {
			fBufferText.RemoveAt(ReceivedData.Num(), fBufferText.Len() - ReceivedData.Num());
		}

		std::string bufferText = TCHAR_TO_UTF8(*fBufferText);
		bufferText = trailingText + bufferText;

		const int lastSeparatorIndex = bufferText.find_last_of(messagesSeparator);
		trailingText = bufferText.substr(lastSeparatorIndex + 1);
		std::string textToParse = bufferText.substr(0, lastSeparatorIndex);

		// Split the string on separator occurence
		std::vector<std::string> splitStrings;
		std::istringstream dataStream(textToParse);
		std::string s;
		while (getline(dataStream, s, messagesSeparator)) {
			if (!s.empty())
				splitStrings.push_back(s);
		}
		
		// Std::strings to WeArtMessages
		messages.resize(splitStrings.size());
		for (int i = 0; i < messages.size(); i++) {
			messages[i] = messageSerializer.Deserialize(splitStrings[i]);
			FString fMessage(splitStrings[i].c_str());
			LogMessage(WeArtMessageType::MessageReceived, fMessage);
		}
		return true;
	}

	messages.clear();
	return false;
}

void UWeArtController::OnMessageReceived(WeArtMessage* msg)
{
	std::string ID = msg->getID();
	if (ID == "exit" || ID == "disconnect") {
		StopConnection();
	}

	std::string id = msg->getID();
	
	if (!StatusTracker)
	{
		return;
	}

	if (!foundStatusTracker)
	{
		return;
	}
	
	if (id == FMiddlewareStatusMessage::ID) 
	{
		StatusTracker->MiddlewareStatusDataCPP = static_cast<FMiddlewareStatusMessage*>(msg)->data();
		StatusTracker->SetMiddlewareStatus(static_cast<FMiddlewareStatusMessage*>(msg)->data());
		StatusTracker->OnMiddlewareSignature.Broadcast();
	}
	else if (id == DevicesStatusMessage::ID)
	{
		std::vector<ConnectedDeviceStatus> devices = static_cast<DevicesStatusMessage*>(msg)->devices();

		for (size_t i = 0; i < devices.size(); i++)
		{
			timeToHideNotUsedHand = true;
			devices[i].handSide == EHandSide::Left
				? isLeftHandConnected = true
				: isRightHandConnected = true;
		}
		
		StatusTracker->SetConnectedDevicesStatus(devices);
		StatusTracker->OnDevicesSignature.Broadcast();	
	}
	else if (id == TdProWeArtAppStatus::ID) 
	{
		StatusTracker->WeartAppStatusDataCPP = static_cast<TdProWeArtAppStatus*>(msg)->data();
		StatusTracker->SetWeartAppData(static_cast<TdProWeArtAppStatus*>(msg)->data());
		StatusTracker->OnMiddlewareSignature.Broadcast();
	}
	else if (id == TdProDevicesStatusMessage::ID)
	{
		std::vector<TdProConnectedDeviceStatus> devices = static_cast<TdProDevicesStatusMessage*>(msg)->getTdProDevices();
		
		for (size_t i = 0; i < devices.size(); i++)
		{
			timeToHideNotUsedHand = true;
			devices[i].handSide == EHandSide::Left
				? isLeftHandConnected = true
				: isRightHandConnected = true;
		}
		
		StatusTracker->SetConnectedTdProDevicesStatus(devices);
		StatusTracker->OnDevicesSignature.Broadcast();
	}
}


void UWeArtController::StopConnection()
{
	IsConnected = false;
	isCancellationRequested = true;
	if (socket) {
		socket->Close();
	}
}


void UWeArtController::LogMessage(WeArtMessageType type, const FString& message)
{
	if (type == WeArtMessageType::MessageReceived)
	{
		HandleReceivedMessage(message);
	}

	if (!DebugMessages) {
		return;
	}

	switch (type) {
	case WeArtMessageType::MessageSent: {
		UE_LOG(LogWeArt, Log, TEXT("To Middleware: %s"), *message);
		break;
	}
	case WeArtMessageType::MessageReceived: {
		UE_LOG(LogWeArt, Log, TEXT("From Middleware: %s"), *message);
		break;
	}
	default:
		break;
	}
}

void UWeArtController::HandleReceivedMessage(const FString& message)
{
	if (!StatusTracker)
	{
		UE_LOG(LogWeArt, Error, TEXT("No Status Tracker"));
		return;
	}
	
	if (message.Contains("CalibrationStatus"))
	{
		TArray<FString> Out;
		message.ParseIntoArray(Out, TEXT(":"), true);

		if (Out[2] == "1")
		{
			if (foundTrackingCalibration)
				TrackingCalibration->OnMiddlewareCalibrationStart.Broadcast(Out[1] == "1" ? EHandSide::Right : EHandSide::Left);

			if (foundCalibrationUX)
				CalibrationUX->OnUXMiddlewareCalibrationStart.Broadcast(Out[1] == "1" ? EHandSide::Right : EHandSide::Left);

			if (foundStatusTracker)
				StatusTracker->OnStatusMiddlewareCalibrationStart.Broadcast(Out[1] == "1" ? EHandSide::Right : EHandSide::Left);
		}

		if (Out[2] == "2")
		{
			if (foundTrackingCalibration)
				TrackingCalibration->OnMiddlewareCalibrationFinish.Broadcast(Out[1] == "1" ? EHandSide::Right : EHandSide::Left);

			if (foundCalibrationUX)
				CalibrationUX->OnUXMiddlewareCalibrationFinish.Broadcast(Out[1] == "1" ? EHandSide::Right : EHandSide::Left);

			if (foundStatusTracker)
				StatusTracker->OnStatusMiddlewareCalibrationFinish.Broadcast(Out[1] == "1" ? EHandSide::Right : EHandSide::Left);
		}
	}

	if (message.StartsWith("CalibrationResult"))
	{
		TArray<FString> Out;
		message.ParseIntoArray(Out, TEXT(":"), true);

		if (Out[2] == "0")
		{

			if (foundTrackingCalibration)
				TrackingCalibration->OnMiddlewareCalibrationResultSuccess.Broadcast(Out[1] == "1" ? EHandSide::Right : EHandSide::Left);

			if (foundCalibrationUX)
				CalibrationUX->OnUXMiddlewareCalibrationResultSuccess.Broadcast(Out[1] == "1" ? EHandSide::Right : EHandSide::Left);

			if (foundStatusTracker)
				StatusTracker->OnStatusMiddlewareCalibrationResultSuccess.Broadcast(Out[1] == "1" ? EHandSide::Right : EHandSide::Left);
		}
		else
		{
			if (foundTrackingCalibration)
				TrackingCalibration->OnMiddlewareCalibrationResultFail.Broadcast(Out[1] == "1" ? EHandSide::Right : EHandSide::Left);

			if (foundCalibrationUX)
				CalibrationUX->OnUXMiddlewareCalibrationResultFail.Broadcast(Out[1] == "1" ? EHandSide::Right : EHandSide::Left);

			if (foundStatusTracker)
				StatusTracker->OnStatusMiddlewareCalibrationResultFail.Broadcast(Out[1] == "1" ? EHandSide::Right : EHandSide::Left);
		}
	}
}

void UWeArtController::LogError(WeArtError error, FString& message) const
{
	if (!DebugMessages) {
		return;
	}

	switch (error) {
	case WeArtError::ConnectionError: {
		UE_LOG(LogWeArt, Error, TEXT("Cannot connect to %s."), *message);
		break;
	}
	case WeArtError::ReceiveMessageError: {
		UE_LOG(LogWeArt, Error, TEXT("Error on receiving message %s"), *message);
		break;
	}
	case WeArtError::SendMessageError: {
		UE_LOG(LogWeArt, Error, TEXT("Error on sending message %s"), *message);
		break;
	}
	default:
		break;
	};
}

void UWeArtController::Tick(float DeltaTime)
{
	
}

void UWeArtController::ScheduleController()
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [&]() {

		FString addrStr;

		while (!isCancellationRequested) {
			// Create a socket and address structure in a platform agnostic way.
			socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
			TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
			
			// Find the IP address of the local ethernet adapter.
			// The socket subsystem itself performs some checks
			// which should filter out most unwanted results.
			// If that's not enough, overriding the following
			// getter might be necessary.
			TArray<TSharedPtr<FInternetAddr>> addresses;
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalAdapterAddresses(addresses);
			if (addresses.Num() < 1 && !addresses[0]) {
				assert(false);
			}
			else {
				uint32 ip;
				addresses[0]->GetIp(ip);
				addr->SetIp(2130706433); //Local host 127.0.0.1 in decimal
				addr->SetPort(ClientPort);
				addrStr = *addr->ToString(true);
			}

			// Attempt to connect to the target.
			if (socket->Connect(*addr)) {
				UE_LOG(LogWeArt, Log, TEXT("Connected to %s."), *addrStr);
				IsConnected = true;

				RequestMiddlewareAndDeviceData();
				
				if (RawDataAutoStart)
				{
					RawDataOn rawDataOn;
					SendMessage(&rawDataOn);
				}

				if (CalibrationAutoStart)
				{
					StartCalibrationMessage startCalibrationMessage;
					SendMessage(&startCalibrationMessage);
				}
			}
			else {
				FString ipString = addr->ToString(true);
				LogError(WeArtError::ConnectionError, ipString);
				StopConnection();
			}

			// Keep receiving messages unless cancelled.
			while (!isCancellationRequested) {
				std::vector<WeArtMessage*> msgs;
				if (ReceiveMessages(msgs)) {
					for (auto msg : msgs) {

						if (msg == nullptr)
							continue;

						OnMessageReceived(msg);

						// Forward the message to relevant tracking objects
						for (UWeArtThimbleTrackingObject* obj : thimbleTrackingObjects)
						{
							obj->OnMessageReceived(msg);
						}

						for (UWeArtThimbleSensorObject* obj : thimbleSensorObjects)
						{
							obj->OnMessageReceived(msg);
						}
					}
				}
			}
		}


		// Log address
		if (socket->GetConnectionState() != SCS_Connected) {
			UE_LOG(LogWeArt, Log, TEXT("Disconnected from %s."), *addrStr);
		}
	}
	);
}

bool UWeArtController::GetOrderToHideHand()
{
	return timeToHideNotUsedHand;
}

bool UWeArtController::GetIsRightHandConnected()
{
	return isRightHandConnected;
}

bool UWeArtController::GetIsLeftHandConnected()
{
	return isLeftHandConnected;
}

bool UWeArtController::IsTickable() const
{
	return true;
}

bool UWeArtController::IsTickableInEditor() const
{
	return false;
}

bool UWeArtController::IsTickableWhenPaused() const
{
	return false;
}

TStatId UWeArtController::GetStatId() const
{
	return TStatId();
}

UWorld* UWeArtController::GetWorld() const
{
	return GetOuter()->GetWorld();
}

bool UWeArtController::AreValidFunctorResources(const AActor* Actor) const
{
	return !IsValid(this) || !IsValid(GetWorld()) || !IsValid(Actor) || StopInitBlueprintActors;
}

template <typename T>
bool BindFoundActor(T*& Actor, bool& bFoundActor, std::function<void(T*)> BindEventsFunc, AActor* PresistentLevelActor)
{
	Actor = Cast<T>(PresistentLevelActor);
	bFoundActor = true;
	BindEventsFunc(Actor);
	return true;
}

// Helper function template to bind events for different actor types
template <typename T>
bool BindActorEvents(T*& Actor, UWorld* World, bool& bFoundActor, std::function<void(T*)> BindEventsFunc)
{
	if (!IsValid(World)) return false;

	// Check actors in the persistent level
	if (IsValid(World->PersistentLevel))  // Check if PersistentLevel is valid
	{
		for (AActor* PersistentLevelActor : World->PersistentLevel->Actors)
		{
			if (IsValid(PersistentLevelActor) && PersistentLevelActor->IsA<T>())  // Ensure the actor is valid
			{
				return BindFoundActor<T>(Actor, bFoundActor, BindEventsFunc, PersistentLevelActor);
			}
		}
	}
	
	for (ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
	{
		if (IsValid(StreamingLevel) && StreamingLevel->IsLevelLoaded())
		{
			if (ULevel* LoadedLevel = StreamingLevel->GetLoadedLevel())
			{
				for (AActor* LevelActor : LoadedLevel->Actors)
				{
					if (IsValid(LevelActor) && LevelActor->IsA<T>())
					{
						return BindFoundActor<T>(Actor, bFoundActor, BindEventsFunc, LevelActor);
					}
				}
			}
		}
	}
	
	return false; // Return false if no actor is found
}

void UWeArtController::RequestMiddlewareAndDeviceData()
{
	if (ControllerAutoStart)
	{
		StartFromClientMessage startMsg;
		SendMessage(&startMsg);
	}
	
	GetMiddlewareStatus middlewareStatus;
	SendMessage(&middlewareStatus);
	
	GetDevicesStatusMessage devicesStatusMessage;
	SendMessage(&devicesStatusMessage);
}

void UWeArtController::InitBlueprintActors()
{
	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]()
	{
		BindActorEvents<AWeArtTrackingCalibration>(TrackingCalibration, GetWorld(), foundTrackingCalibration, [this](AWeArtTrackingCalibration* Actor)
			{
				if (AreValidFunctorResources(Actor)) return;
				Actor->OnMiddlewareCalibrationStart.AddUniqueDynamic(Actor, &AWeArtTrackingCalibration::OnCalibrationStart);
				Actor->OnMiddlewareCalibrationFinish.AddUniqueDynamic(Actor, &AWeArtTrackingCalibration::OnCalibrationFinish);
				Actor->OnMiddlewareCalibrationResultSuccess.AddUniqueDynamic(Actor, &AWeArtTrackingCalibration::OnCalibrationResultSuccess);
				Actor->OnMiddlewareCalibrationStop.AddUniqueDynamic(Actor, &AWeArtTrackingCalibration::OnCalibrationStop);
				Actor->OnMiddlewareCalibrationResultFail.AddUniqueDynamic(Actor, &AWeArtTrackingCalibration::OnCalibrationResultFail);
			});

			BindActorEvents<AWeArtCalibrationUX>(CalibrationUX, GetWorld(), foundCalibrationUX, [this](AWeArtCalibrationUX* Actor)
			{
				if (AreValidFunctorResources(Actor)) return;
				Actor->OnUXMiddlewareCalibrationStart.AddUniqueDynamic(Actor, &AWeArtCalibrationUX::OnUXCalibrationStart);
				Actor->OnUXMiddlewareCalibrationFinish.AddUniqueDynamic(Actor, &AWeArtCalibrationUX::OnUXCalibrationFinish);
				Actor->OnUXMiddlewareCalibrationResultSuccess.AddUniqueDynamic(Actor, &AWeArtCalibrationUX::OnUXCalibrationResultSuccess);
				Actor->OnUXMiddlewareCalibrationResultFail.AddUniqueDynamic(Actor, &AWeArtCalibrationUX::OnUXCalibrationResultFail);
				Actor->OnUXMiddlewareCalibrationStop.AddUniqueDynamic(Actor, &AWeArtCalibrationUX::OnUXCalibrationStop);
			});

			BindActorEvents<AWeArtStatusTracker>(StatusTracker, GetWorld(), foundStatusTracker, [this](AWeArtStatusTracker* Actor)
			{
				if (AreValidFunctorResources(Actor)) return;
				Actor->OnStatusMiddlewareCalibrationStart.AddUniqueDynamic(Actor, &AWeArtStatusTracker::OnStatusCalibrationStart);
				Actor->OnStatusMiddlewareCalibrationFinish.AddUniqueDynamic(Actor, &AWeArtStatusTracker::OnStatusCalibrationFinish);
				Actor->OnStatusMiddlewareCalibrationResultSuccess.AddUniqueDynamic(Actor, &AWeArtStatusTracker::OnStatusCalibrationResultSuccess);
				Actor->OnStatusMiddlewareCalibrationResultFail.AddUniqueDynamic(Actor, &AWeArtStatusTracker::OnStatusCalibrationResultFail);
				Actor->OnStatusMiddlewareCalibrationStop.AddUniqueDynamic(Actor, &AWeArtStatusTracker::OnStatusCalibrationStop);
				Actor->OnMiddlewareSignature.AddUniqueDynamic(Actor, &AWeArtStatusTracker::OnMiddlewareStatus);
				Actor->OnDevicesSignature.AddUniqueDynamic(Actor, &AWeArtStatusTracker::OnDevicesStatus);
			});
	});
}

bool UWeArtController::GetGesturesEnabled()
{
	return GesturesEnabled;
}
