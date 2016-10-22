/* OptionsWindow.h
 * Copyright 2010 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#ifndef SERVICES_OPTIONSWINDOW_H
#define SERVICES_OPTIONSWINDOW_H

#include <InterfaceKit.h>
#include <Application.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"


class OptionsWindow : public BWindow {
public:
							OptionsWindow(BMessage*);
	virtual	bool			QuitRequested();
			void			Show(BRect);
			bool			GetDrawTwoLines();
			int				GetIconSize();
			float			GetFontSize();
			window_look		GetWindowLook();
			window_feel		GetFloat();
			AppOptions		GetOptions();
			void			GetOptionsToSave(BMessage*);
private:
	BPopUpMenu				*fIconSizeMenu, *fFontSizeMenu, *fWindowLookMenu;
	BMenuField				*fIconSizeMF, *fFontSizeMF, *fWindowLookMF;
	BCheckBox				*fLinesCB, *fFloatCB;
	BButton					*fAboutB;
	void					_SetSavedOptions(BMessage*);
};

#endif
