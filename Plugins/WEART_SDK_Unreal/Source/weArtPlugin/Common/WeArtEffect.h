#pragma once

#include "WeArtTemperature.h"
#include "WeArtForce.h"
#include "WeArtTexture.h"
#include "WeArtEffect.generated.h"

 /// <summary>
 /// A general effect function interface.
 /// </summary>
 UCLASS(BlueprintType)
class WEARTPLUGIN_API UWeArtEffect : public UObject{

 	GENERATED_BODY()
public:
 	UFUNCTION(BlueprintCallable, Category = "WeArt Effects")
	virtual FWeArtTemperature	getTemperature();
 	UFUNCTION(BlueprintCallable, Category = "WeArt Effects")
	virtual FWeArtForce			getForce();
 	UFUNCTION(BlueprintCallable, Category = "WeArt Effects")
	virtual FWeArtTexture		getTexture();
};
