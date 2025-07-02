#pragma once

#include "CoreMinimal.h"
#include "WeArtCommon.h"
#include "WeArtTemperature.generated.h"

/// <summary>
/// Temperature value to be applied to an effect
/// </summary>
USTRUCT(BlueprintType)
struct WEARTPLUGIN_API FWeArtTemperature {
	GENERATED_BODY()

public:
	/** Whether the temperature is active. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (InlineEditConditionToggle))
		bool active;

	// The value 0.5 is room temperature. The value 0 is cold and value 1 is hot.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Value", EditCondition = "active", ClampMin = "0.0", ClampMax = "1.0"))
		float value;

	/** Default constructor initializes temperature as inactive with default value. */
	FWeArtTemperature() : active(false), value(WeArtConstants::defaultTemperature) {};

	/**
	 * @brief Equality operator for comparing two temperature objects.
	 * @param other The other temperature object to compare.
	 * @return True if the two temperature objects are equal, false otherwise.
	 */
	bool operator==(const FWeArtTemperature& other) const {
		return (active == other.active && value == other.value);
	};
	
	/**
	 * @brief Inequality operator for comparing two temperature objects.
	 * @param other The other temperature object to compare.
	 * @return True if the two temperature objects are not equal, false otherwise.
	 */
	bool operator!=(const FWeArtTemperature& other) const {
		return (active != other.active || value != other.value);
	};
};


