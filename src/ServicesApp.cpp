/* ServicesApp.cpp
 * Copyright 2010 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#include "ServicesApp.h"

ServicesApp::ServicesApp()
	:
	BApplication(app_signature)
{
	// Get saved settings
	BPath settingsPath;
	find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath);
	settingsPath.Append(settings_filename);
	BFile settingsFile;
	BMessage savedOptions;
	BRect mainWindowRect(0,0,200,400);
	status_t frameResult;
	status_t result = settingsFile.SetTo(settingsPath.Path(), B_READ_ONLY);
	if(result == B_OK)
	{
		savedOptions.Unflatten(&settingsFile);
		frameResult = savedOptions.FindRect(NAME_WINDOW_FRAME, &mainWindowRect);
	}
	
	// Create Windows
	fOptions = new OptionsWindow(&savedOptions);
	AppOptions options = fOptions->GetOptions();
	window_look look = fOptions->GetWindowLook();
	window_feel feel = fOptions->GetFloat();
	fWindow = new MainWindow(mainWindowRect, look, feel, options);
	if(frameResult != B_OK)
		fWindow->CenterOnScreen();
	fWindow->Show();
}


bool
ServicesApp::QuitRequested()
{
	BMessage options;
	fOptions->GetOptionsToSave(&options);
	options.AddRect(NAME_WINDOW_FRAME, fWindow->Frame());
	BPath settingsPath;
	find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath);
	settingsPath.Append(settings_filename);
	BFile settingsFile(settingsPath.Path(), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	options.Flatten(&settingsFile);
	return true;
}


void
ServicesApp::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case LINES_OPTION_CHANGED: {
			bool twoLines = fOptions->GetDrawTwoLines();
			msg->AddBool(NAME_LINES_OPTION, twoLines);
			fWindow->OptionsChanged(msg);
			break;
		}
		case ICON_OPTION_CHANGED: {
			int8 iconSize = fOptions->GetIconSize();
			msg->AddInt8(NAME_ICON_OPTION, iconSize);
			fWindow->OptionsChanged(msg);
			break;
		}
		case FONT_OPTION_CHANGED: {
			float fontSize = fOptions->GetFontSize();
			msg->AddFloat(NAME_FONT_OPTION, fontSize);
			fWindow->OptionsChanged(msg);
			break;
		}
		case LOOK_OPTION_CHANGED: {
			window_look look = fOptions->GetWindowLook();
			fWindow->Lock();
			fWindow->SetLook(look);
			fWindow->Unlock();
			break;
		}
		case FLOAT_OPTION_CHANGED: {
			window_feel feel = fOptions->GetFloat();
			fWindow->Lock();
			fWindow->SetFeel(feel);
			fWindow->Unlock();
			break;
		}
		case SHOW_OPTIONS:
		{
			if(fOptions->IsHidden())
				fOptions->Show(fWindow->Frame());
			fOptions->Activate();
			break;
		}
		default:
			BApplication::MessageReceived(msg);
			break;
	}
}


void
ServicesApp::AboutRequested()
{
	(new BAlert("About Services Watcher",
				"Services Watcher version "SW_version"\n"
				"Copyright 2010 Brian Hill\n\n"
				"This program will monitor the status of\n"
				"system services.\n\n"
				"Contact information and the latest version\n"
				"of this application can be found at\n"
				"http://www.haikuware.com", "OK"))->Go(NULL);
}


int main()
{
	ServicesApp myApp;
	myApp.Run();
	return 0;
}
