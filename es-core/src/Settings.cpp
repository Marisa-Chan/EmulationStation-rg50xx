#include "Settings.h"

#include "utils/FileSystemUtil.h"
#include "Log.h"
#include "Scripting.h"
#include "platform.h"
#include <pugixml.hpp>
#include <algorithm>
#include <vector>

Settings* Settings::sInstance = NULL;

// these values are NOT saved to es_settings.xml
// since they're set through command-line arguments, and not the in-program settings menu
std::vector<const char*> settings_dont_save {
	"Debug",
	"DebugGrid",
	"DebugText",
	"DebugImage",
	"ForceKid",
	"ForceKiosk",
	"IgnoreGamelist",
	"HideConsole",
	"ShowExit",
	"ConfirmQuit",
	"SplashScreen",
	"VSync",
	"FullscreenBorderless",
	"Windowed",
	"WindowWidth",
	"WindowHeight",
	"ScreenWidth",
	"ScreenHeight",
	"ScreenOffsetX",
	"ScreenOffsetY",
	"ScreenRotate",
	"MonitorID"
};

Settings::Settings()
{
	setDefaults();
	loadFile();
}

Settings* Settings::getInstance()
{
	if(sInstance == NULL)
		sInstance = new Settings();

	return sInstance;
}

void Settings::setDefaults()
{
	mBoolMap.clear();
	mIntMap.clear();

	mBoolMap["BackgroundJoystickInput"] = false;
	mBoolMap["ParseGamelistOnly"] = false;
	mBoolMap["ShowHiddenFiles"] = false;
	mBoolMap["DrawFramerate"] = false;
	mBoolMap["ShowExit"] = true;
	mBoolMap["ConfirmQuit"] = true;
	mBoolMap["FullscreenBorderless"] = false;
	mBoolMap["Windowed"] = false;
	mBoolMap["SplashScreen"] = true;
	mStringMap["StartupSystem"] = "";
	mBoolMap["DisableKidStartMenu"] = true;

	mBoolMap["VSync"] = true;

	mBoolMap["EnableSounds"] = true;
	mBoolMap["ShowHelpPrompts"] = true;
	mBoolMap["DoublePressRemovesFromFavs"] = false;
	mBoolMap["ScrapeRatings"] = true;
	mBoolMap["IgnoreGamelist"] = false;
	mBoolMap["HideConsole"] = true;
	mBoolMap["QuickSystemSelect"] = true;
	mBoolMap["MoveCarousel"] = true;

	mBoolMap["ThreadedLoading"] = false;

	mBoolMap["Debug"] = false;
	mBoolMap["DebugGrid"] = false;
	mBoolMap["DebugText"] = false;
	mBoolMap["DebugImage"] = false;

	mIntMap["ScreenSaverTime"] = 5 * Settings::ONE_MINUTE_IN_MS;
	mIntMap["SystemSleepTime"] = 0 * Settings::ONE_MINUTE_IN_MS;
	mBoolMap["SystemSleepTimeHintDisplayed"] = false;
	mIntMap["ScraperResizeWidth"] = 400;
	mIntMap["ScraperResizeHeight"] = 0;
	#ifdef _RPI_
		mIntMap["MaxVRAM"] = 80;
	#else
		mIntMap["MaxVRAM"] = 100;
	#endif

	mStringMap["TransitionStyle"] = "fade";
	mStringMap["ThemeSet"] = "";
	mStringMap["ScreenSaverBehavior"] = "dim";
	mStringMap["Scraper"] = "TheGamesDB";
	mStringMap["GamelistViewStyle"] = "automatic";
	mStringMap["SaveGamelistsMode"] = "on exit";

	mBoolMap["ScreenSaverControls"] = true;
	mStringMap["ScreenSaverGameInfo"] = "never";
	mBoolMap["StretchVideoOnScreenSaver"] = false;
	mStringMap["PowerSaverMode"] = "disabled";

	mIntMap["ScreenSaverSwapMediaTimeout"] = 10000;
	mBoolMap["SlideshowScreenSaverStretch"] = false;
	mStringMap["SlideshowScreenSaverBackgroundAudioFile"] = Utils::FileSystem::getHomePath() + "/configs/emulationstation/slideshow/audio/slideshow_bg.wav";
	mBoolMap["SlideshowScreenSaverCustomMediaSource"] = false;
	mStringMap["SlideshowScreenSaverMediaDir"] = Utils::FileSystem::getHomePath() + "/configs/emulationstation/slideshow/media";
	mStringMap["SlideshowScreenSaverImageFilter"] = ".png,.jpg";
	mStringMap["SlideshowScreenSaverVideoFilter"] = ".mp4,.avi";
	mBoolMap["SlideshowScreenSaverRecurse"] = false;

	// This setting only applies to raspberry pi but set it for all platforms so
	// we don't get a warning if we encounter it on a different platform
	mBoolMap["VideoOmxPlayer"] = false;
	#ifdef _OMX_
		// we're defaulting to OMX Player for full screen video on the Pi
		mBoolMap["ScreenSaverOmxPlayer"] = true;
		// use OMX Player defaults
		mStringMap["SubtitleFont"] = "/usr/share/fonts/truetype/freefont/FreeSans.ttf";
		mStringMap["SubtitleItalicFont"] = "/usr/share/fonts/truetype/freefont/FreeSansOblique.ttf";
		mIntMap["SubtitleSize"] = 55;
		mStringMap["SubtitleAlignment"] = "left";
	#else
		mBoolMap["ScreenSaverOmxPlayer"] = false;
	#endif

	mIntMap["ScreenSaverSwapVideoTimeout"] = 30000;

	mBoolMap["VideoAudio"] = true;
	mBoolMap["ScreenSaverVideoMute"] = false;
	mStringMap["VlcScreenSaverResolution"] = "original";
	// Audio out device for Video playback using OMX player.
	mStringMap["OMXAudioDev"] = "both";
	mIntMap["RandomCollectionMaxGames"] = 0; // 0 == no limit
	std::map<std::string, int> m1;
	mMapIntMap["RandomCollectionSystemsAuto"] = m1;
	std::map<std::string, int> m2;
	mMapIntMap["RandomCollectionSystemsCustom"] = m2;
	std::map<std::string, int> m3;
	mMapIntMap["RandomCollectionSystems"] = m3;
	mStringMap["RandomCollectionExclusionCollection"] = "";
	mStringMap["CollectionSystemsAuto"] = "";
	mStringMap["CollectionSystemsCustom"] = "";
	mStringMap["DefaultScreenSaverCollection"] = "";
	mBoolMap["CollectionShowSystemInfo"] = true;
	mBoolMap["SortAllSystems"] = false;
	mBoolMap["UseCustomCollectionsSystem"] = true;
	mBoolMap["BackgroundIndexing"] = false;

	mBoolMap["LocalArt"] = false;

	// Audio out device for volume control
	#ifdef _RPI_
		mStringMap["AudioDevice"] = "HDMI";
	#else
		mStringMap["AudioDevice"] = "Master";
	#endif

	mStringMap["AudioCard"] = "default";
	mStringMap["UIMode"] = "Full";
	mStringMap["UIMode_passkey"] = "uuddlrlrba";
	mBoolMap["ForceKiosk"] = false;
	mBoolMap["ForceKid"] = false;
	mBoolMap["ForceDisableFilters"] = false;

	mIntMap["WindowWidth"]   = 0;
	mIntMap["WindowHeight"]  = 0;
	mIntMap["ScreenWidth"]   = 0;
	mIntMap["ScreenHeight"]  = 0;
	mIntMap["ScreenOffsetX"] = 0;
	mIntMap["ScreenOffsetY"] = 0;
	mIntMap["ScreenRotate"]  = 0;
	mIntMap["MonitorID"] = 0;

	mBoolMap["UseFullscreenPaging"] = false;

	mBoolMap["IgnoreLeadingArticles"] = false;
	//No spaces!  Order is important!
	//"The A Squad" given [a,an,the] will sort as "A Squad", but given [the,a,an] will sort as "Squad"
	mStringMap["LeadingArticles"] = "a,an,the";
	
	mIntMap["AudioVolume"] = 50;
	mIntMap["Brightness"] = 70;
}

template <typename K, typename V>
void saveMap(pugi::xml_document& doc, std::map<K, V>& map, const char* type)
{
	for(auto iter = map.cbegin(); iter != map.cend(); iter++)
	{
		// key is on the "don't save" list, so don't save it
		if(std::find(settings_dont_save.cbegin(), settings_dont_save.cend(), iter->first) != settings_dont_save.cend())
			continue;

		pugi::xml_node node = doc.append_child(type);
		node.append_attribute("name").set_value(iter->first.c_str());
		node.append_attribute("value").set_value(iter->second);
	}
}

void Settings::saveFile()
{
	LOG(LogDebug) << "Settings::saveFile() : Saving Settings to file.";
	const std::string path = Utils::FileSystem::getHomePath() + "/configs/emulationstation/es_settings.cfg";

	pugi::xml_document doc;

	saveMap<std::string, bool>(doc, mBoolMap, "bool");
	saveMap<std::string, int>(doc, mIntMap, "int");
	saveMap<std::string, float>(doc, mFloatMap, "float");

	//saveMap<std::string, std::string>(doc, mStringMap, "string");
	for(auto iter = mStringMap.cbegin(); iter != mStringMap.cend(); iter++)
	{
		pugi::xml_node node = doc.append_child("string");
		node.append_attribute("name").set_value(iter->first.c_str());
		node.append_attribute("value").set_value(iter->second.c_str());
	}

	for(auto &m : mMapIntMap)
	{
		pugi::xml_node node = doc.append_child("map");
		node.append_attribute("name").set_value(m.first.c_str());
		std::string datatype = "int";
		node.append_attribute("type").set_value(datatype.c_str());
		for(auto &intMap : m.second) // intMap is a <string, int> map
		{
			pugi::xml_node entry = node.append_child(datatype.c_str());
			entry.append_attribute("name").set_value(intMap.first.c_str());
			entry.append_attribute("value").set_value(intMap.second);
		}
	}

	doc.save_file(path.c_str());

	Scripting::fireEvent("config-changed");
	Scripting::fireEvent("settings-changed");
}

void Settings::loadFile()
{
	const std::string path = Utils::FileSystem::getHomePath() + "/configs/emulationstation/es_settings.cfg";

	if(!Utils::FileSystem::exists(path))
		return;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(path.c_str());
	if(!result)
	{
		LOG(LogError) << "Could not parse Settings file!\n   " << result.description();
		return;
	}

	for(pugi::xml_node node = doc.child("bool"); node; node = node.next_sibling("bool"))
		setBool(node.attribute("name").as_string(), node.attribute("value").as_bool());
	for(pugi::xml_node node = doc.child("int"); node; node = node.next_sibling("int"))
		setInt(node.attribute("name").as_string(), node.attribute("value").as_int());
	for(pugi::xml_node node = doc.child("float"); node; node = node.next_sibling("float"))
		setFloat(node.attribute("name").as_string(), node.attribute("value").as_float());
	for(pugi::xml_node node = doc.child("string"); node; node = node.next_sibling("string"))
		setString(node.attribute("name").as_string(), node.attribute("value").as_string());

	for(pugi::xml_node node = doc.child("map"); node; node = node.next_sibling("map"))
	{
		std::string mapName = node.attribute("name").as_string();
		std::string mapType = node.attribute("type").as_string();
		if (mapType == "int") {
			// only supporting int value maps currently
			std::map<std::string, int> _map;
			for(pugi::xml_node entry : node.children(mapType.c_str()))
			{
				_map[entry.attribute("name").as_string()] = entry.attribute("value").as_int();
			}
			setMap(mapName, _map);
		} else {
			LOG(LogWarning) << "Map: '" << mapName << "'. Unsupported data type '"<< mapType <<"'. Value ignored!";
		}
	}

	processBackwardCompatibility();
}


void Settings::setMap(const std::string& key, const std::map<std::string, int>& map)
{
	mMapIntMap[key] = map;
}

const std::map<std::string, int> Settings::getMap(const std::string& key)
{
	if(mMapIntMap.find(key) == mMapIntMap.cend())
	{
		LOG(LogError) << "Tried to use undefined setting " << key << "!";
		std::map<std::string, int> empty;
		return empty;

	}
	return mMapIntMap[key];
}


template<typename Map>
void Settings::renameSetting(Map& map, std::string&& oldName, std::string&& newName)
{
	typename Map::const_iterator it = map.find(oldName);
	if (it != map.end()) {
		map[newName] = it->second;
		map.erase(it);
	}
}

void Settings::processBackwardCompatibility()
{
	{	// SaveGamelistsOnExit -> SaveGamelistsMode
		std::map<std::string, bool>::const_iterator it = mBoolMap.find("SaveGamelistsOnExit");
		if (it != mBoolMap.end()) {
			mStringMap["SaveGamelistsMode"] = it->second ? "on exit" : "never";
			mBoolMap.erase(it);
		}
	}

	{ // ScreenSaverSlideShow Image -> Media
		renameSetting<std::map<std::string, int>>(mIntMap, std::string("ScreenSaverSwapImageTimeout"), std::string("ScreenSaverSwapMediaTimeout"));
		renameSetting<std::map<std::string, bool>>(mBoolMap, std::string("SlideshowScreenSaverCustomImageSource"), std::string("SlideshowScreenSaverCustomMediaSource"));
		renameSetting<std::map<std::string, std::string>>(mStringMap, std::string("SlideshowScreenSaverImageDir"), std::string("SlideshowScreenSaverMediaDir"));
	}
}


//Print a warning message if the setting we're trying to get doesn't already exist in the map, then return the value in the map.
#define SETTINGS_GETSET(type, mapName, getMethodName, setMethodName) type Settings::getMethodName(const std::string& name) \
{ \
	if(mapName.find(name) == mapName.cend()) \
	{ \
		LOG(LogError) << "Tried to use unset setting " << name << "!"; \
	} \
	return mapName[name]; \
} \
void Settings::setMethodName(const std::string& name, type value) \
{ \
	mapName[name] = value; \
}

SETTINGS_GETSET(bool, mBoolMap, getBool, setBool);
SETTINGS_GETSET(int, mIntMap, getInt, setInt);
SETTINGS_GETSET(float, mFloatMap, getFloat, setFloat);
SETTINGS_GETSET(const std::string&, mStringMap, getString, setString);
