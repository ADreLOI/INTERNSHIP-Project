#include "WeArtHapticObject.h"
#include "WeArtController.h"
#include <algorithm>

#include "WeArtUtilities.h"


UWeArtHapticObject::UWeArtHapticObject() : handSideFlag(0), actuationPointFlag(0), ActiveEffects{}, Touched(false),
                                           SentStop(false)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeArtHapticObject::AddEffect(UWeArtEffect* effect)
{
	if (!ContainsEffect(effect))
	{
		ActiveEffects.Add(effect);
		UpdateEffects();
	}
}


void UWeArtHapticObject::RemoveEffect(UWeArtEffect* effect)
{
	if (ContainsEffect(effect))
	{
		ActiveEffects.Remove(effect);
		UpdateEffects();
	}
}


bool UWeArtHapticObject::ContainsEffect(UWeArtEffect* effect)
{
	return ActiveEffects.Contains(effect);
}

void UWeArtHapticObject::RemoveLastEffect() 
{
	if (ActiveEffects.Num())
	{
		ActiveEffects.RemoveAt(ActiveEffects.Num()-1);
	}
}

void UWeArtHapticObject::ClearEffects() 
{
	ActiveEffects.Empty();
	UpdateEffects();
}

TArray<UWeArtEffect*> UWeArtHapticObject::GetActiveEffects() const
{
	return ActiveEffects;
}

void UWeArtHapticObject::HandleSendingTemperature()
{
	UWeArtEffect* lastActiveTemperature = nullptr;

	for (int32 i = 0; i < ActiveEffects.Num(); i++)
	{
		if (ActiveEffects[i]->getTemperature().active)
		{
			lastActiveTemperature = ActiveEffects[i];
		}
	}
	
	FWeArtTemperature newTemperature = lastActiveTemperature ? lastActiveTemperature->getTemperature() : FWeArtTemperature();
	
	if (newTemperature != weArtTemperature) {
		if (!newTemperature.active) {
			SendMessage(new StopTemperatureMessage());
		}
		else {
			SendMessage(new SetTemperatureMessage(newTemperature.value));
		}
		
		weArtTemperature = newTemperature;
	}
}


void UWeArtHapticObject::HandleSendingForce()
{
	UWeArtEffect* lastActiveForceEffect = nullptr;

	for (int32 i = 0; i < ActiveEffects.Num(); i++)
	{
		if (ActiveEffects[i]->getForce().active)
		{
			lastActiveForceEffect = ActiveEffects[i];
		}
	}
	
	FWeArtForce newForce = lastActiveForceEffect ? lastActiveForceEffect->getForce() : FWeArtForce();
	
	if (newForce != weArtForce) {
		if (!newForce.active) {
			SendMessage(new StopForceMessage());
		}
		else {
			const float fValue[3] = { newForce.value, 0.0f, 0.0f };
			SendMessage(new SetForceMessage(fValue));
		}
		
		weArtForce = newForce;
	}
}

void UWeArtHapticObject::HandleSendingTexture()
{
	UWeArtEffect* lastActiveTextureEffect = nullptr;

	for (int32 i = 0; i < ActiveEffects.Num(); i++)
	{
		if (ActiveEffects[i]->getTexture().active)
		{
			lastActiveTextureEffect = ActiveEffects[i];
		}
	}
	
	FWeArtTexture newTex = lastActiveTextureEffect ? lastActiveTextureEffect->getTexture() : FWeArtTexture();
	
	if (newTex != weArtTexture) {
		if (!newTex.active) {
			SendMessage(new StopTextureMessage());
		}
		else {
			const float texValue[3] = { WeArtConstants::defaultTextureVelocity[0], WeArtConstants::defaultTextureVelocity[1], newTex.isForcedTextureVelocity ? WeArtConstants::staticTextureVelocity : newTex.textureVelocity };
			SendMessage(new SetTextureMessage(static_cast<int>(newTex.textureType), texValue, newTex.volume));
		}
		weArtTexture = newTex;
	}
}

void UWeArtHapticObject::SendStopEffects()
{
	weArtForce.value = WeArtConstants::defaultForce;
	weArtForce.active = false;
	weArtTemperature.value = WeArtConstants::defaultTemperature;
	weArtTemperature.active = false;
	weArtTexture.textureType = static_cast<TextureType>(WeArtConstants::defaultTextureIndex);
	weArtTexture.textureVelocity = WeArtConstants::defaultTextureVelocity[2];
	weArtTexture.active = false;

	StopTemperatureMessage msg1;
	SendMessage(&msg1);
	StopForceMessage msg2;
	SendMessage(&msg2);
	StopTextureMessage msg3;
	SendMessage(&msg3);
}

void UWeArtHapticObject::UpdateEffects()
{
	if (ActiveEffects.IsEmpty() && !SentStop) {
		SendStopEffects();

		SentStop = true;
	}
	else if (!ActiveEffects.IsEmpty()){

		// Update temperature and send relevant messages
		HandleSendingTemperature();

		// Update force and set relevant messages
		HandleSendingForce();

		// Update texture and set relevant messages
		HandleSendingTexture();

		SentStop = false;
	}
}

void UWeArtHapticObject::SendMessage(WeArtMessage* msg) const
{
	UGameInstance* GameInstance = GetOuter()->GetWorld()->GetGameInstance();
	UWeArtController* weArtController = GameInstance->GetSubsystem<UWeArtController>();
	if (!weArtController) {
		return;
	}

	TEnumRange<EActuationPoint> actuationPoints = TEnumRange<EActuationPoint>();
	
	for (EHandSide hs : TEnumRange<EHandSide>()) {
		if (static_cast<uint8_t>(hs) & handSideFlag) {
			for (EActuationPoint ap : actuationPoints) {
				if (static_cast<uint8_t>(ap) & actuationPointFlag) {
					msg->setHandSide(hs);
					msg->setActuationPoint(ap);
					weArtController->SendMessage(msg);
				}
			}
		}
	}
}

int32 UWeArtHapticObject::GetActuationPoint() const
{
	return actuationPointFlag;
}
