/* MainBListView.cpp
 * Copyright 2010 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#include "MainBListView.h"

MainBListView::MainBListView()
	:
	BListView(BRect(), "Services List")
{
	fMenu = new BPopUpMenu(B_EMPTY_STRING, false, false);
	startStopMI = new BMenuItem("Start", new BMessage());
	fMenu->AddItem(startStopMI);
	restartMI = new BMenuItem("Restart", new BMessage(RESTART_SERVICE));
	fMenu->AddItem(restartMI);
	fMenu->AddSeparatorItem();
	optionsMI = new BMenuItem("Options" B_UTF8_ELLIPSIS, new BMessage(SHOW_OPTIONS));
	fMenu->AddItem(optionsMI);
}


void
MainBListView::AttachedToWindow()
{
	BListView::AttachedToWindow();
	fMenu->SetTargetForItems(this);
	optionsMI->SetTarget(be_app);
}


void
MainBListView::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case RESTART_SERVICE:
		case START_SERVICE:
		case STOP_SERVICE:
		{
			int32 selectedIndex = CurrentSelection();
			if(selectedIndex < 0)
				return;
			ServiceListItem *selectedItem = (ServiceListItem*)ItemAt(selectedIndex);
			selectedItem->ProcessMessage(msg);
			InvalidateItem(selectedIndex);
			break;
		}
		default:
			BListView::MessageReceived(msg);
	}
}


void
MainBListView::MouseDown(BPoint pos)
{
	BListView::MouseDown(pos);
	
	int32 button;
	Looper()->CurrentMessage()->FindInt32("buttons", &button);
	if (button & B_SECONDARY_MOUSE_BUTTON) {
		int32 selectedIndex = CurrentSelection();
		if(selectedIndex < 0)
			return;
		ServiceListItem *selectedItem = (ServiceListItem*)ItemAt(selectedIndex);
		int appState = selectedItem->GetState();
		
		switch(appState)
		{
			case STATE_OK_RUNNING:
			{
				restartMI->SetEnabled(true);
				startStopMI->SetLabel("Stop");
				startStopMI->SetEnabled(true);
				startStopMI->Message()->what = STOP_SERVICE;
				break;
			}
			case STATE_OK_STOPPED:
			{
				restartMI->SetEnabled(false);
				startStopMI->SetLabel("Start");
				startStopMI->SetEnabled(true);
				startStopMI->Message()->what = START_SERVICE;
				break;
			}
			case STATE_OK_STARTING:
			case STATE_OK_RESTARTING:
			case STATE_OK_STOPPING:
			{
				restartMI->SetEnabled(false);
				startStopMI->SetEnabled(false);
				break;
			}
			case STATE_ERROR_STARTING__IS_NOT_RUNNING:
			case STATE_ERROR_STOPPING__IS_NOT_RUNNING:
			{
				restartMI->SetEnabled(false);
				startStopMI->SetLabel("Start");
				startStopMI->SetEnabled(true);
				startStopMI->Message()->what = START_SERVICE;
				break;
			}
			case STATE_ERROR_STARTING__IS_RUNNING:
			case STATE_ERROR_STOPPING__IS_RUNNING:
			{
				restartMI->SetEnabled(true);
				startStopMI->SetLabel("Stop");
				startStopMI->SetEnabled(true);
				startStopMI->Message()->what = STOP_SERVICE;
				break;
			}
			default:
			{
				restartMI->SetEnabled(false);
				startStopMI->SetLabel("Stop");
				startStopMI->SetEnabled(false);
				startStopMI->Message()->what = 0;
			}
		}
		
		ConvertToScreen(&pos);
		fMenu->Go(pos, true, true, BRect(pos.x - 2, pos.y - 2,
			pos.x + 2, pos.y + 2), true);
	}
}


void
MainBListView::OptionsChanged(BMessage *msg)
{
	Window()->Lock();
	switch(msg->what)
	{
		case LINES_OPTION_CHANGED:
		{
			bool twoLines;
			msg->FindBool(NAME_LINES_OPTION, &twoLines);
			int count = CountItems();
			for(int i=0; i<count; i++)
			{
				ServiceListItem* item = (ServiceListItem*)(ItemAt(0));
				item->SetDrawTwoLines(twoLines);
				RemoveItem(item);
				AddItem(item);
			}
			break;
		}
		case ICON_OPTION_CHANGED:
		{
			int8 iconSize;
			msg->FindInt8(NAME_ICON_OPTION, &iconSize);
			int count = CountItems();
			for(int i=0; i<count; i++)
			{
				ServiceListItem* item = (ServiceListItem*)(ItemAt(0));
				item->SetIconSize(iconSize);
				RemoveItem(item);
				AddItem(item);
			}
			break;
		}
		case FONT_OPTION_CHANGED:
		{
			float fontSize;
			msg->FindFloat(NAME_FONT_OPTION, &fontSize);
			SetFontSize(fontSize);
			int count = CountItems();
			for(int i=0; i<count; i++)
			{
				ServiceListItem* item = (ServiceListItem*)(ItemAt(0));
				RemoveItem(item);
				AddItem(item);
			}
			break;
		}
	}
	Window()->Unlock();
}

