#include "BrightnessControl.h"

#include "math/Misc.h"
#include "Log.h"
#include "Settings.h"


std::weak_ptr<BrightnessControl> BrightnessControl::sInstance;

#define MIN_BRIGHTNESS 1
#define MAX_BRIGHTNESS 171
#define BRIGHT_STEP ((float)(MAX_BRIGHTNESS - MIN_BRIGHTNESS) / 100.0)


BrightnessControl::BrightnessControl()
{
	init();
}

BrightnessControl::BrightnessControl(const BrightnessControl & right)
{
	(void)right;
	sInstance = right.sInstance;
}

BrightnessControl & BrightnessControl::operator=(const BrightnessControl & right)
{
	if (this != &right) {
		sInstance = right.sInstance;
	}

	return *this;
}

BrightnessControl::~BrightnessControl()
{
	//set original volume levels for system
	//setVolume(originalVolume);

	deinit();
}

std::shared_ptr<BrightnessControl> & BrightnessControl::getInstance()
{
	//check if an BrightnessControl instance is already created, if not create one
	static std::shared_ptr<BrightnessControl> sharedInstance = sInstance.lock();
	if (sharedInstance == nullptr) {
		sharedInstance.reset(new BrightnessControl);
		sInstance = sharedInstance;
	}
	return sharedInstance;
}

void BrightnessControl::init()
{
	//initialize audio mixer interface
	//try to open mixer device
	originalVal = readBrightness();
	internalVal = originalVal / BRIGHT_STEP;
}

void BrightnessControl::deinit()
{
	//deinitialize audio mixer interface
	//writeBrightness(0);
}

int BrightnessControl::getBrightness() const
{
	return internalVal;
}

void BrightnessControl::setBrightness(int val)
{
	//clamp to 0-100 range
	if (val < 0)
		val = 0;
	
	if (val > 100)
		val = 100;
	
	internalVal = val;
	
	int newval = ((float)val * BRIGHT_STEP);
	if (newval != originalVal)
	{
		originalVal = newval;
		writeBrightness(originalVal);
	}
}

int BrightnessControl::readBrightness()
{
  int val = 0;
  int fd = open("/sys/class/backlight/backlight/brightness", O_RDONLY);
  if (fd > 0)
  {
    char buf[32];
	int num = read(fd, buf, 31);
	close(fd);
	if (num > 0)
	{
	  buf[num] = 0;
	  val = atoi(buf);
	}
  }

  val = (MAX_BRIGHTNESS - val);
  if (val < 0)
  	val = 0;
  else if (val > (MAX_BRIGHTNESS - MIN_BRIGHTNESS))
  	val = (MAX_BRIGHTNESS - MIN_BRIGHTNESS);

  return val;
}

void BrightnessControl::writeBrightness(int val)
{
  if (val > (MAX_BRIGHTNESS - MIN_BRIGHTNESS))
    val = (MAX_BRIGHTNESS - MIN_BRIGHTNESS);
  else if (val < 0)
    val = 0;

  val = MAX_BRIGHTNESS - val;
  
  int fd = open("/sys/class/backlight/backlight/brightness", O_WRONLY);
  if (fd > 0)
  {
    char buf[32];
	int num = sprintf(buf, "%d", val);
	write(fd, buf, num);
	close(fd);
  }
}
