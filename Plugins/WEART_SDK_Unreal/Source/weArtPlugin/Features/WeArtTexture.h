#pragma once

#include "CoreMinimal.h"
#include "WeArtCommon.h"
#include "WeArtTexture.generated.h"

/// <summary>
/// Texture information to be applied to an effect
/// </summary>
USTRUCT(BlueprintType)
struct FWeArtTexture {
	GENERATED_BODY()

	/** Whether the texture is active. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool active;

	/** The type of texture. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Index", EditCondition = "active", ClampMin = "0", ClampMax = "21"))
		TextureType textureType;
	
	/** Whether the texture velocity is forced. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "ForcedVelocity", EditCondition = "active"))
		bool isForcedTextureVelocity = false;

	// The value 0 represents no texture feedback and value 1 the maximum intensity.
	UPROPERTY(BlueprintReadWrite, Meta = (DisplayName = "TexutreVelocity", EditCondition = "active", ClampMin = "0.0", ClampMax = "1.0"))
		float textureVelocity = 0.0f;

	/** The volume of the texture. */
	UPROPERTY(NoClear, BlueprintReadWrite, Meta = (DisplayName = "Volume", EditCondition = "active", ClampMin = "0.0", ClampMax = "1.0"))
		float volume;

	/** Default constructor initializes texture with default values. */
	FWeArtTexture() : active(false),
		textureType((TextureType)WeArtConstants::defaultTextureIndex),
		isForcedTextureVelocity(false),
		textureVelocity(WeArtConstants::defaultTextureVelocity[2]),
		volume(WeArtConstants::defaultVolumeTexture)
	{};

	/**
	 * @brief Equality operator for comparing two texture objects.
	 * @param other The other texture object to compare.
	 * @return True if the two texture objects are equal, false otherwise.
	 */
	bool operator==(const FWeArtTexture& other) const {
		return (active == other.active && textureType == other.textureType && textureVelocity == other.textureVelocity && volume == other.volume && isForcedTextureVelocity == other.isForcedTextureVelocity);
	};

	/**
	 * @brief Inequality operator for comparing two texture objects.
	 * @param other The other texture object to compare.
	 * @return True if the two texture objects are not equal, false otherwise.
	 */
	bool operator!=(const FWeArtTexture& other) const {
		return (active != other.active || textureType != other.textureType || textureVelocity != other.textureVelocity || volume != other.volume || isForcedTextureVelocity != other.isForcedTextureVelocity);
	};
};

