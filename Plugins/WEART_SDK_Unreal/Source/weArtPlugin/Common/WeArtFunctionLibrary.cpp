
#include "WeArtFunctionLibrary.h"
#include "WeArtSettings.h"


const UWeArtSettings* UWeArtFunctionLibrary::GetWeArtSettings()
{
	return GetMutableDefault<UWeArtSettings>();
}

bool UWeArtFunctionLibrary::MatchRegex(const FString& InputString, const FString& Pattern)
{
	FRegexPattern RegexPattern(Pattern);
	FRegexMatcher RegexMatcher(RegexPattern, InputString);
	return RegexMatcher.FindNext();
}
