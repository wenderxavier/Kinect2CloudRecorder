#pragma once

#include <string>

/**
 * @brief: Abstracts the configuration file to a well known interface
 * that's used throught the code. 
 * In its current implementation, it makes use of ofxXmlSettings in
 * order to read from an xml file and update its local data.
 */

class ConfigurationManager
{
public:
	ConfigurationManager(void);
	~ConfigurationManager(void);
	
	//  =========== Basics ==========

	bool load(const std::string& filename);
	bool save(const std::string& filename);
	bool save();

	//  =========== Screen ==========

	void setScreenWidth(unsigned int value)
	{
		if (value > 0) { screenWidth = value; }
	}

	void setScreenHeight(unsigned int value)
	{
		if (value > 0) { screenHeight = value; }
	}

	unsigned int getScreenWidth() { return screenWidth; }
	unsigned int getScreenHeight(){ return screenHeight;}

private:
	// Basics
	std::string filename;

	// Screen
	unsigned int screenWidth, screenHeight;
};

