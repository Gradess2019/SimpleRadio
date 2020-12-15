// Gradess Games. All rights reserved.

#include "RadioStationPlaylist.h"
#include "Kismet/KismetSystemLibrary.h"

TArray<FString> URadioStationPlaylist::GetStreams()
{
	auto Streams = TArray<FString>();
	
	GConfig->GetArray(
		TEXT("RadioStationPlaylist"),
		TEXT("StreamURL"), Streams,
		FPaths::ProjectPluginsDir().Append("SimpleRadio/Config/Radio.ini")
	);

	UE_LOG(LogTemp, Warning, TEXT("Streams path: %s"), *FPaths::ProjectPluginsDir().Append("SimpleRadio/Config/Radio.ini"))
	return Streams;
}
