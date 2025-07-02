
#pragma once

#include "CoreMinimal.h"
#include "WeArtEffect.h"
#include "WeArtTouchEffect.generated.h"

USTRUCT(BlueprintType)
struct FUWeArtImpactInfo
{
	GENERATED_BODY()
public:
	static const FUWeArtImpactInfo none;

	UPROPERTY(BlueprintReadWrite)
	bool valid;
	UPROPERTY(BlueprintReadWrite)
	FVector position;
	UPROPERTY(BlueprintReadWrite)
	float time;
	UPROPERTY(BlueprintReadWrite)
	float multiplier;
};

/// <summary>
/// Effect to be applied to the thimble
/// </summary>
UCLASS(BlueprintType)
class UWeArtTouchEffect : public UWeArtEffect
{
	GENERATED_BODY()
public:

	UWeArtTouchEffect();
	
	UPROPERTY(BlueprintReadWrite, Category = "WeArt Effects")
	FWeArtTemperature effTemperature;
	UPROPERTY(BlueprintReadWrite, Category = "WeArt Effects")
	FWeArtForce effForce;
	UPROPERTY(BlueprintReadWrite, Category = "WeArt Effects")
	FWeArtTexture effTexture;
	UPROPERTY(BlueprintReadWrite, Category = "WeArt Effects")
	FUWeArtImpactInfo lastWeArtImpactInfo;

	UFUNCTION(BlueprintCallable, Category = "WeArt Effects")
	void Init (FWeArtTemperature temp, FWeArtForce force, FWeArtTexture texture)
	{
		effTemperature = temp;
		effForce = force;
		effTexture = texture;
		lastWeArtImpactInfo = FUWeArtImpactInfo::none;
	};

	UFUNCTION(BlueprintCallable, Category = "WeArt Effects")
	bool Set(FWeArtTemperature temp, FWeArtForce force, FWeArtTexture texture, FUWeArtImpactInfo impactInfo);

	virtual FWeArtTemperature	getTemperature()	override { return effTemperature; }
	virtual FWeArtForce			getForce()	override { return effForce; }
	virtual FWeArtTexture		getTexture() override { return effTexture; }
};
