#include "WeArtTouchEffect.h"
#include "WeArtCommon.h"

UWeArtTouchEffect::UWeArtTouchEffect()
{
}

bool UWeArtTouchEffect::Set(FWeArtTemperature temp, FWeArtForce force, FWeArtTexture texture, FUWeArtImpactInfo impactInfo)
{
    bool changed = false;

    // Temperature
    changed |= (effTemperature != temp);
    effTemperature = temp;

    // Force
    changed |= (effForce != force);
    effForce = force;

    // Texture
    if (lastWeArtImpactInfo.valid && impactInfo.valid)
    {
        float dx = FVector::Distance(impactInfo.position, lastWeArtImpactInfo.position) / 100.0f;
        float dt = FMath::Max<float>(FLT_EPSILON, impactInfo.time - lastWeArtImpactInfo.time);
        float slidingSpeed = impactInfo.multiplier * (dx / dt);

        if (texture.isForcedTextureVelocity == false)
        {
            texture.textureVelocity = slidingSpeed;
        }
        else
        {
            texture.textureVelocity = WeArtConstants::staticTextureVelocity;
        }
       
    }
    
    lastWeArtImpactInfo = impactInfo;
    changed |= !(effTexture == texture);
    effTexture = texture;

    return changed;
}
