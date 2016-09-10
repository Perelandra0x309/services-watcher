/* OptionsWindow.cpp
 * Copyright 2010 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#include "OptionsWindow.h"

OptionsWindow::OptionsWindow(BMessage* savedOptions)
	:
	BWindow(BRect(), "Services Watcher Options", B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE)
{
	Lock();
	fOneLineRB = new BRadioButton("One Line", "Show application name and status on one line",
		new BMessage(LINES_OPTION_CHANGED));
	fTwoLinesRB = new BRadioButton("Two Lines", "Show application name and status on two lines",
		new BMessage(LINES_OPTION_CHANGED));
	
	fIconSizeMenu = new BPopUpMenu("Icon Size Menu");
	fIconSizeMenu->AddItem(new BMenuItem("No Icon", new BMessage(ICON_OPTION_CHANGED)));
	fIconSizeMenu->AddItem(new BMenuItem("16", new BMessage(ICON_OPTION_CHANGED)));
	fIconSizeMenu->AddItem(new BMenuItem("24", new BMessage(ICON_OPTION_CHANGED)));
	fIconSizeMenu->AddItem(new BMenuItem("32", new BMessage(ICON_OPTION_CHANGED)));
	fIconSizeMenu->AddItem(new BMenuItem("48", new BMessage(ICON_OPTION_CHANGED)));
	fIconSizeMenu->AddItem(new BMenuItem("64", new BMessage(ICON_OPTION_CHANGED)));
	fIconSizeMF = new BMenuField("Icon Size Field", "Icon size:", fIconSizeMenu);
	
	fFontSizeMenu = new BPopUpMenu("Font Size Menu");
	fFontSizeMenu->AddItem(new BMenuItem("System size", new BMessage(FONT_OPTION_CHANGED)));
	BString fontSize;
	for(int i=8; i<25; i++)
	{
		fontSize.SetTo("");
		fontSize << i;
		fFontSizeMenu->AddItem(new BMenuItem(fontSize.String(), new BMessage(FONT_OPTION_CHANGED)));
	}
	fFontSizeMF = new BMenuField("Font Size Field", "Font size:", fFontSizeMenu);
	
	fWindowLookMenu = new BPopUpMenu("Window Look Menu");
	fWindowLookMenu->AddItem(new BMenuItem("Normal Title Bar", new BMessage(LOOK_OPTION_CHANGED)));
	fWindowLookMenu->AddItem(new BMenuItem("Small Title Bar", new BMessage(LOOK_OPTION_CHANGED)));
	fWindowLookMenu->AddItem(new BMenuItem("No Title Bar", new BMessage(LOOK_OPTION_CHANGED)));
	fWindowLookMenu->AddItem(new BMenuItem("No Border", new BMessage(LOOK_OPTION_CHANGED)));
	fWindowLookMF = new BMenuField("Window Look Field", "Window Look:", fWindowLookMenu);
	
	fFloatCB = new BCheckBox("Float Above All Windows", new BMessage(FLOAT_OPTION_CHANGED));
	
	fAboutB = new BButton("About", "About" B_UTF8_ELLIPSIS, new BMessage(B_ABOUT_REQUESTED));
	
	fOneLineRB->SetTarget(be_app);
	fTwoLinesRB->SetTarget(be_app);
	fIconSizeMenu->SetTargetForItems(be_app);
	fFontSizeMenu->SetTargetForItems(be_app);
	fWindowLookMenu->SetTargetForItems(be_app);
	fFloatCB->SetTarget(be_app);
	fAboutB->SetTarget(be_app);
	
	SetLayout(new BGroupLayout(B_VERTICAL));
	AddChild(BGroupLayoutBuilder(B_VERTICAL, 3)
		.Add(fOneLineRB)
		.Add(fTwoLinesRB)
		.Add(BGroupLayoutBuilder(B_HORIZONTAL)
			.Add(fIconSizeMF)
			.AddGlue()
		)
		.Add(BGroupLayoutBuilder(B_HORIZONTAL)
			.Add(fFontSizeMF)
			.AddGlue()
		)
		.Add(BGroupLayoutBuilder(B_HORIZONTAL)
			.Add(fWindowLookMF)
			.AddGlue()
		)
		.Add(fFloatCB)
		.Add(BGroupLayoutBuilder(B_HORIZONTAL)
			.AddGlue()
			.Add(fAboutB)
		)
		.AddGlue()
		.SetInsets(3, 3, 3, 3)
	);
	
	BSize size = ChildAt(0)->PreferredSize();
	ResizeTo(size.width, size.height);
	_SetSavedOptions(savedOptions);
	
	Unlock();
}


bool
OptionsWindow::QuitRequested()
{
	Hide();
	return false;
}


void
OptionsWindow::Show(BRect mainWindowFrame)
{
	// detect if there is enough room in the main screen
	BPoint windowLeftTop(mainWindowFrame.right + 10, mainWindowFrame.top);
	BScreen screen;
	if(screen.IsValid())
	{
		BRect screenFrame = screen.Frame();
		float availableWidth = screenFrame.right - mainWindowFrame.right;
		if(availableWidth < Frame().Width())
			windowLeftTop.x = screenFrame.right - Frame().Width() - 5;
		float availableHeight = screenFrame.bottom - mainWindowFrame.top;
		if(availableHeight < Frame().Height())
			windowLeftTop.y = screenFrame.bottom - Frame().Height() - 5;
	}
	MoveTo(windowLeftTop.x, windowLeftTop.y);
	BWindow::Show();
}


bool
OptionsWindow::GetDrawTwoLines()
{
	if(fOneLineRB->Value())
		return false;
	else if(fTwoLinesRB->Value())
		return true;
	else
		return true;
}


int
OptionsWindow::GetIconSize()
{
	int size = 0;
	BMenuItem *item = fIconSizeMenu->FindMarked();
	if(item != NULL && fIconSizeMenu->IndexOf(item) != 0)
		size = atoi(item->Label());
	return size;
}


float
OptionsWindow::GetFontSize()
{
	float size = 0;
	BMenuItem *item = fFontSizeMenu->FindMarked();
	if(item != NULL && fFontSizeMenu->IndexOf(item) != 0)
		size = atoi(item->Label());
	else
		size = be_plain_font->Size();
	return size;
}


window_look
OptionsWindow::GetWindowLook()
{
	window_look look = B_TITLED_WINDOW_LOOK;
	BMenuItem *item = fWindowLookMenu->FindMarked();
	if(item != NULL)
	{
		switch(fWindowLookMenu->IndexOf(item))
		{
			case 1: {
				look = B_FLOATING_WINDOW_LOOK;
				break;
			}
			case 2: {
				look = B_MODAL_WINDOW_LOOK;
				break;
			}
			case 3: {
				look = B_BORDERED_WINDOW_LOOK;
				break;
			}
		}
	}
	return look;
}


window_feel
OptionsWindow::GetFloat()
{
	if(fFloatCB->Value())
		return B_FLOATING_ALL_WINDOW_FEEL;
	else
		return B_NORMAL_WINDOW_FEEL;
}


AppOptions
OptionsWindow::GetOptions()
{
	AppOptions options;
	options.drawTwoLines = GetDrawTwoLines();
	options.iconSize = GetIconSize();
	options.fontSize = GetFontSize();
	return options;

}


// Options to save to file
void
OptionsWindow::GetOptionsToSave(BMessage* message)
{
	message->AddBool(NAME_LINES_OPTION, GetDrawTwoLines());
	
	int8 markedItem;
	BMenuItem *item = fIconSizeMenu->FindMarked();
	if(item != NULL)
		markedItem = fIconSizeMenu->IndexOf(item);
	else
		markedItem = 3;
	message->AddInt8(NAME_ICON_OPTION, markedItem);
	
	item = fFontSizeMenu->FindMarked();
	if(item != NULL)
		markedItem = fFontSizeMenu->IndexOf(item);
	else
		markedItem = 0;
	message->AddInt8(NAME_FONT_OPTION, markedItem);
	
	item = fWindowLookMenu->FindMarked();
	if(item != NULL)
		markedItem = fWindowLookMenu->IndexOf(item);
	else
		markedItem = 0;
	message->AddInt8(NAME_LOOK_OPTION, markedItem);
	
	message->AddBool(NAME_FEEL_OPTION, fFloatCB->Value());
}


// Options read from file
void
OptionsWindow::_SetSavedOptions(BMessage *options)
{
	bool boolValue;
	status_t result = options->FindBool(NAME_LINES_OPTION, &boolValue);
	if(result != B_OK)
		boolValue = true;
	if(boolValue)
		fTwoLinesRB->SetValue(true);
	else
		fOneLineRB->SetValue(true);
	
	int8 markedItem;
	result = options->FindInt8(NAME_ICON_OPTION, &markedItem);
	if(result != B_OK)
		markedItem = 3;
	BMenuItem *item = fIconSizeMenu->ItemAt(markedItem);
	if(item != NULL)
		item->SetMarked(true);
	
	result = options->FindInt8(NAME_FONT_OPTION, &markedItem);
	if(result != B_OK)
		markedItem = 0;
	item = fFontSizeMenu->ItemAt(markedItem);
	if(item != NULL)
		item->SetMarked(true);
	
	result = options->FindInt8(NAME_LOOK_OPTION, &markedItem);
	if(result != B_OK)
		markedItem = 0;
	item = fWindowLookMenu->ItemAt(markedItem);
	if(item != NULL)
		item->SetMarked(true);
	
	result = options->FindBool(NAME_FEEL_OPTION, &boolValue);
	if(result != B_OK)
		boolValue = false;
	fFloatCB->SetValue(boolValue);
}
