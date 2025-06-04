#pragma once
#ifndef ES_APP_BRIGHTNESS_CONTROL_H
#define ES_APP_BRIGHTNESS_CONTROL_H


#include <unistd.h>
#include <fcntl.h>
#include <memory>

/*!
Singleton pattern. Call getInstance() to get an object.
*/
class BrightnessControl
{
	int originalVal = 0;
	int internalVal = 0;

	static std::weak_ptr<BrightnessControl> sInstance;

	BrightnessControl();
	BrightnessControl(const BrightnessControl & right);
    BrightnessControl & operator=(const BrightnessControl & right);

public:
	static std::shared_ptr<BrightnessControl> & getInstance();

	void init();
	void deinit();

	int getBrightness() const;
	void setBrightness(int val);

	~BrightnessControl();

private:
	static int readBrightness();
	static void writeBrightness(int vol);
};

#endif // ES_APP_BRIGHTNESS_CONTROL_H
