/* constants.h
 * Copyright 2010 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#ifndef SERVICES_CONSTANTS_H
#define SERVICES_CONSTANTS_H

#define SW_version "1.0.1"
static const char* app_signature = "application/x-vnd.BH-ServicesWatcher";
static const char* settings_filename = "ServicesWatcher_settings";
const int32 kIconDefaultSize = 32;
const int kIconMargin = 2;
const int kTextMargin = 2;

struct AppOptions {
	bool drawTwoLines;
	int iconSize;
	float fontSize;
};

//Messages
enum {
	RESTART_SERVICE,
	START_SERVICE,
	STOP_SERVICE,
	SHOW_OPTIONS,
	LINES_OPTION_CHANGED,
	ICON_OPTION_CHANGED,
	FONT_OPTION_CHANGED,
	LOOK_OPTION_CHANGED,
	FLOAT_OPTION_CHANGED
};

#define NAME_LINES_OPTION "linesOption"
#define NAME_ICON_OPTION "iconSize"
#define NAME_FONT_OPTION "fontSize"
#define NAME_WINDOW_FRAME "windowFrame"
#define NAME_LOOK_OPTION "windowLook"
#define NAME_FEEL_OPTION "windowFeel"

#endif
