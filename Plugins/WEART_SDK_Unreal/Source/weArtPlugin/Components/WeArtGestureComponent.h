// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeArtCommon.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "WeArtGestureComponent.generated.h"

USTRUCT(BlueprintType)
struct WEARTPLUGIN_API FGesture
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName GestureName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> IndexClosures;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> MiddleClosures;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> ThumbClosures;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> AnnularClosures;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> PinkyClosures;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool CheckHandRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FRotator> HandRotations;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool WithDelay = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = WithDelay))
	float Delay = 2.f;
	
	bool StartedTimer = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGestureComplete, const FName&, GestureName);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEARTPLUGIN_API UWeArtGestureComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UWeArtHandController* HandControllerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<FGesture> Gestures;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TEnumAsByte<EHandSide> HandSide;
	
	FName TeleportGesture = "Teleport Activation";
	FName LastGestureName;
	FName SavedGestureName;
	bool StartedTimer = false;
	
	FGesture* SavedGesture;
	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true), Category = Debugging)
	bool DebugGestureClosures;

	void OnTimerExceed();
	
public:	
	UWeArtGestureComponent();
	
	bool AreClosuresRanged(const FGesture& Gesture, bool IsTdPro) const;
	void HandleGestures();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool EnabledTeleportation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true)) 
	bool EnabledInformationPanel;
	
	UPROPERTY(BlueprintAssignable, Category = "Gestures")
	FOnGestureComplete OnGestureUniqueChanged;

	UFUNCTION(BlueprintPure, BlueprintCallable)
	FName GetLastGestureName() const;
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FName GetSavedGestureName() const;
protected:
	virtual void BeginPlay() override;
	void LogGestures(float IndexClosure, float MiddleClosure, float ThumbClosure, bool IsTouchDiverPro,
	                 float AnnularClosure, float PinkyClosure, bool indexInRange, bool middleInRange, bool thumbInRange,
	                 bool annularInRange, bool pinkyInRange) const;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
