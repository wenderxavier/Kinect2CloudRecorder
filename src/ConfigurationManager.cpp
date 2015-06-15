#include "ConfigurationManager.h"

#include <ofxXmlSettings.h>
#include <ofLog.h>

// tag names
const std::string ConfScreenWidth("screen:width");
const std::string ConfScreenHeight("screen:height");

// default values
const unsigned int ConfScreenWidthDefault = 1024;
const unsigned int ConfScreenHeightDefault = 768;

ConfigurationManager::ConfigurationManager(void)
{
	screenWidth = ConfScreenWidthDefault;
	screenHeight= ConfScreenHeightDefault;
}

ConfigurationManager::~ConfigurationManager(void)
{
}

bool ConfigurationManager::load(const std::string& f)
{
	ofxXmlSettings configFile;

	if (f.empty())
		return false;

	if (configFile.load(f))
	{
		filename = f;
		
		// screen
		setScreenWidth(configFile.getValue(ConfScreenWidth,   (int) ConfScreenWidthDefault));
		setScreenHeight(configFile.getValue(ConfScreenHeight, (int) ConfScreenHeightDefault));

	} else {
		ofLogWarning("Couldn't load configuration file ",f.c_str());
	}
}

bool ConfigurationManager::save(const std::string& f)
{
	ofxXmlSettings configFile;

	// screen
	configFile.setValue(ConfScreenWidth,   (int) ConfScreenWidthDefault);
	configFile.setValue(ConfScreenHeight,  (int) ConfScreenHeightDefault);

	// save
	return configFile.saveFile(f);
}

bool ConfigurationManager::save()
{
	if (filename.empty())
	{
		return false;
	}
	return save(filename);
}


