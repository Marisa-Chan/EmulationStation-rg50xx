#pragma once
#ifndef ES_APP_VOLUME_CONTROL_H
#define ES_APP_VOLUME_CONTROL_H


#include <unistd.h>
#include <fcntl.h>
#include <memory>

/*!
Singleton pattern. Call getInstance() to get an object.
*/
class VolumeControl
{
	int originalVolume = 0;
	int internalVolume = 0;

	static std::weak_ptr<VolumeControl> sInstance;

	VolumeControl();
	VolumeControl(const VolumeControl & right);
    VolumeControl & operator=(const VolumeControl & right);

public:
	static std::shared_ptr<VolumeControl> & getInstance();

	void init();

	int getVolume() const;
	void setVolume(int volume);

private:
	static int readVolume();
	static void writeVolume(int vol);
};

#endif // ES_APP_VOLUME_CONTROL_H
