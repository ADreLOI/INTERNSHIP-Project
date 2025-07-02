#include "WeArtThimbleTrackingObject.h"


#include "WeArtController.h"

UWeArtThimbleTrackingObject::UWeArtThimbleTrackingObject(): handSide(EHandSide::Right), actuationPoint(EActuationPoint::Thumb),
                                                            Closure(0.f), Abduction(0.f),
                                                            WeArtController(nullptr)
{
}


UWeArtThimbleTrackingObject::~UWeArtThimbleTrackingObject()
{
}
 
void UWeArtThimbleTrackingObject::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* gameInstance = GetOuter()->GetWorld()->GetGameInstance();
	if (!gameInstance) {
		return;
	}

	WeArtController = gameInstance->GetSubsystem<UWeArtController>();
	if (!WeArtController) {
		return;
	}

	WeArtController->thimbleTrackingObjects.Add(this);
}


void UWeArtThimbleTrackingObject::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UGameInstance* gameInstance = GetOuter()->GetWorld()->GetGameInstance();
	if (!gameInstance) {
		return;
	}

	UWeArtController* weArtController = gameInstance->GetSubsystem<UWeArtController>();
	if (!weArtController) {
		return;
	}

	weArtController->thimbleTrackingObjects.Remove(this);
}

void UWeArtThimbleTrackingObject::OnMessageReceived(WeArtMessage* msg)
{
	IFingerDataContainable* FingerData = nullptr;
	
	EDeviceGeneration DiverType = WeArtController->DeviceGeneration;
	const std::string msgID = msg->getID();

	if (DiverType == EDeviceGeneration::TouchDiverPro && msgID == "TRACKING_BENDING_G2")
	{
		// Safe cast to FTdProTrackingMessage
		FTdProTrackingMessage* TdProTrackingMessage = static_cast<FTdProTrackingMessage*>(msg);
		FingerData = TdProTrackingMessage;
	}
	else if (msgID == "Tracking")
	{
		TrackingMessage* TdTrackingMessage = static_cast<TrackingMessage*>(msg);
		FingerData = TdTrackingMessage;
	}
	
	if (FingerData)
	{
		Closure = FingerData->GetClosure(handSide, actuationPoint);
		Abduction = FingerData->GetAbduction(handSide, actuationPoint);
	}
}

float UWeArtThimbleTrackingObject::GetClosure() const
{
	return Closure;
}

float UWeArtThimbleTrackingObject::GetAbduction() const
{
	return Abduction;
}

TEnumAsByte<EHandSide> UWeArtThimbleTrackingObject::GetHandSide() const
{
	return handSide;
}
