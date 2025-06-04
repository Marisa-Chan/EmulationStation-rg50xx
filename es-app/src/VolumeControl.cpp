#include "VolumeControl.h"

#include "math/Misc.h"
#include "Log.h"
#include "Settings.h"

std::weak_ptr<VolumeControl> VolumeControl::sInstance;


VolumeControl::VolumeControl()
{
	init();
}

VolumeControl::VolumeControl(const VolumeControl & right)
{
	(void)right;
	sInstance = right.sInstance;
}

VolumeControl & VolumeControl::operator=(const VolumeControl & right)
{
	if (this != &right) {
		sInstance = right.sInstance;
	}

	return *this;
}

VolumeControl::~VolumeControl()
{
	//set original volume levels for system
	//setVolume(originalVolume);

	deinit();
}

std::shared_ptr<VolumeControl> & VolumeControl::getInstance()
{
	//check if an VolumeControl instance is already created, if not create one
	static std::shared_ptr<VolumeControl> sharedInstance = sInstance.lock();
	if (sharedInstance == nullptr) {
		sharedInstance.reset(new VolumeControl);
		sInstance = sharedInstance;
	}
	return sharedInstance;
}

void VolumeControl::init()
{
	//initialize audio mixer interface
	//try to open mixer device
	originalVolume = readVolume();
	internalVolume = originalVolume * 3.226;
}

void VolumeControl::deinit()
{
	//deinitialize audio mixer interface
	writeVolume(0);
}

int VolumeControl::getVolume() const
{
	return internalVolume;
}

void VolumeControl::setVolume(int volume)
{
	//clamp to 0-100 range
	if (volume < 0)
		volume = 0;
	
	if (volume > 100)
		volume = 100;
	
	internalVolume = volume;
	
	int newvol = (float)volume / 3.225;
	if (newvol != originalVolume)
	{
		originalVolume = newvol;
		writeVolume(originalVolume);
	}
}

int VolumeControl::readVolume()
{
  int vol = 0;
  int fd = open("/sys/codec-spk-ctl/vol-ctl", O_RDONLY);
  if (fd > 0)
  {
    char buf[32];
	int num = read(fd, buf, 31);
	close(fd);
	if (num > 0)
	{
	  buf[num] = 0;
	  vol = atoi(buf);
	}
  }
  return vol;
}

void VolumeControl::writeVolume(int vol)
{
  if (vol > 31)
    vol = 31;
  else if (vol < 0)
    vol = 0;
  
  int fd = open("/sys/codec-spk-ctl/vol-ctl", O_WRONLY);
  if (fd > 0)
  {
    char buf[32];
	int num = sprintf(buf, "%d", vol);
	write(fd, buf, num);
	close(fd);
  }
}
