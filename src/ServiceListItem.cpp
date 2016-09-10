/* ServiceListItem.cpp
 * Copyright 2010 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#include "ServiceListItem.h"


ServiceListItem::ServiceListItem(BEntry entry, const char * sig, AppOptions *options)
	:
	BListItem(),
	fCurrentState(STATE_NONE),
	fInitStatus(B_ERROR)
{
	// Draw Options
	fDrawTwoLines = options->drawTwoLines;
	fIconSize = options->iconSize;
	fIcon = NULL;
	
	if(!entry.Exists())
		return;
	
	BPath path;
	entry.GetPath(&path);
	fName.SetTo(path.Leaf());
	fSignature.SetTo(sig);
	entry.GetRef(&fEntryRef);
	
	_GetIcon();
	fInitStatus = B_OK;
}


ServiceListItem::~ServiceListItem()
{
	delete fIcon;
}


void
ServiceListItem::ProcessMessage(BMessage* msg)
{
	switch(msg->what)
	{
		case B_SOME_APP_QUIT:
		{
			const char* sig;
			if (msg->FindString("be:signature", &sig) != B_OK) break;
			if(fSignature.Compare(sig)==0)
			{
				if(fCurrentState == STATE_OK_RESTARTING)
				// In the middle of a restart- send message to start the service
				{
					BMessage message(START_SERVICE);
					ProcessMessage(&message);
				}
				else
				// All other states
				{
					_SetState(STATE_OK_STOPPED);
				}
			}
			break;
		}
		case B_SOME_APP_LAUNCHED:
		{
			const char* sig;
			if (msg->FindString("be:signature", &sig) != B_OK) break;
			if(fSignature.Compare(sig)==0)
			{
				_SetState(STATE_OK_RUNNING);
			}
			break;
		}
		case RESTART_SERVICE:
		{
			_SetState(STATE_OK_RESTARTING);
			status_t rc = _StopService();
			if(rc != B_OK)
			{
				if(be_roster->IsRunning(fSignature.String()))
				{	_SetState(STATE_ERROR_STOPPING__IS_RUNNING); }
				else
				{
					_SetState(STATE_ERROR_STOPPING__IS_NOT_RUNNING);
					// Attempt to start the service
					BMessage message(START_SERVICE);
					ProcessMessage(&message);
				}
			}
			break;
		}
		case START_SERVICE:
		{
			_SetState(STATE_OK_STARTING);
			status_t rc = _StartService();
			if(rc != B_OK)
			{
				if(be_roster->IsRunning(fSignature.String()))
				{	_SetState(STATE_ERROR_STARTING__IS_RUNNING); }
				else
				{	_SetState(STATE_ERROR_STARTING__IS_NOT_RUNNING); }
			}
			break;
		}
		case STOP_SERVICE:
		{
			_SetState(STATE_OK_STOPPING);
			status_t rc = _StopService();
			if(rc != B_OK)
			{
				if(be_roster->IsRunning(fSignature.String()))
				{	_SetState(STATE_ERROR_STOPPING__IS_RUNNING); }
				else
				{	_SetState(STATE_ERROR_STOPPING__IS_NOT_RUNNING); }
			}
			break;
		}
	}
}



void
ServiceListItem::SetDrawTwoLines(bool value)
{
	fDrawTwoLines = value;
}


void
ServiceListItem::SetIconSize(int value)
{
	fIconSize = value;
	_GetIcon();
}


void
ServiceListItem::SetRunningState()
{
	if(be_roster->IsRunning(fSignature.String()))
	{	_SetState(STATE_OK_RUNNING); }
	else
	{	_SetState(STATE_OK_STOPPED); }
}


void
ServiceListItem::_GetIcon()
{
	delete fIcon;
	if(fIconSize == 0)
	{
		fIcon = NULL;
		return;
	}
	BNode node;
	BNodeInfo nodeInfo;
	if (node.SetTo(&fEntryRef) == B_OK) {
		BRect iconRect(0, 0, fIconSize - 1, fIconSize - 1);
		fIcon = new BBitmap(iconRect, 0, B_RGBA32);
		status_t result = BIconUtils::GetVectorIcon(&node, "BEOS:ICON", fIcon);
		if(result != B_OK)
		{
			// attempt to get mini or large icon
			delete fIcon;
			if(nodeInfo.SetTo(&node) == B_OK)
			{
				if(fIconSize<32)
				{
					iconRect.Set(0, 0, 15, 15);
					fIcon = new BBitmap(iconRect, 0, B_RGBA32);
					result = nodeInfo.GetTrackerIcon(fIcon, B_MINI_ICON);
				}
				else
				{
					iconRect.Set(0, 0, 31, 31);
					fIcon = new BBitmap(iconRect, 0, B_RGBA32);
					result = nodeInfo.GetTrackerIcon(fIcon, B_LARGE_ICON);
				}
				if(result != B_OK)
				{
					delete fIcon;
					fIcon = NULL;
				}
			}
			else
				fIcon = NULL;
		}
	}
	else {
		fIcon = NULL;
	}
}


status_t
ServiceListItem::_StartService()
{
	status_t rc = be_roster->Launch(fSignature.String());
	return rc;
}


status_t
ServiceListItem::_StopService()
{
	status_t rc = B_ERROR;
	BMessenger appMessenger(fSignature.String());
	if(!appMessenger.IsValid())
	{
		BString err("Error: Could not create messenger for service ");
		err.Append(fName);
		(new BAlert("", err.String(),"OK"))->Go(NULL);
		return rc;
	}
	rc = appMessenger.SendMessage(B_QUIT_REQUESTED);
	return rc;
}


void
ServiceListItem::_SetStatusText(const char* text)
{
	fStatusText.SetTo("Status: ");
	fStatusText.Append(text);
}


// TODO move this code to a state machine object
void
ServiceListItem::_SetState(int newstate)
{
	switch(newstate)
	{
		case STATE_OK_RUNNING:
		{
			_SetStatusText("Running");
			fCurrentState = STATE_OK_RUNNING;
			break;
		}
		case STATE_OK_STOPPING:
		{
			_SetStatusText("Stopping...");
			fCurrentState = STATE_OK_STOPPING;
			break;
		}
		case STATE_ERROR_STOPPING__IS_NOT_RUNNING:
		{
			_SetStatusText("There was an error stopping the service");
			fCurrentState = STATE_ERROR_STOPPING__IS_NOT_RUNNING;
			break;
		}
		case STATE_ERROR_STOPPING__IS_RUNNING:
		{
			_SetStatusText("There was an error stopping the service");
			fCurrentState = STATE_ERROR_STOPPING__IS_RUNNING;
			break;
		}
		case STATE_OK_STOPPED:
		{
			_SetStatusText("Stopped");
			fCurrentState = STATE_OK_STOPPED;
			break;
		}
		case STATE_OK_STARTING:
		{
			_SetStatusText("Starting...");
			fCurrentState = STATE_OK_STARTING;
			break;
		}
		case STATE_ERROR_STARTING__IS_RUNNING:
		{
			_SetStatusText("There was an error starting the service");
			fCurrentState = STATE_ERROR_STARTING__IS_RUNNING;
		}
		case STATE_ERROR_STARTING__IS_NOT_RUNNING:
		{
			_SetStatusText("There was an error starting the service");
			fCurrentState = STATE_ERROR_STARTING__IS_NOT_RUNNING;
			break;
		}
		case STATE_OK_RESTARTING:
		{
			_SetStatusText("Restarting...");
			fCurrentState = STATE_OK_RESTARTING;
			break;
		}
		default:
		{
			_SetStatusText("Unknown");
			fCurrentState = STATE_NONE;
		}
	}
}


void
ServiceListItem::DrawItem(BView *owner, BRect item_rect, bool complete = false)
{
	float offset_width = 0, offset_height = fFontAscent;
	float listItemHeight = Height();
	rgb_color backgroundColor;
	
	//background
	if(IsSelected()) {
		backgroundColor = ui_color(B_MENU_SELECTED_BACKGROUND_COLOR);
	}
	else {
		// alternate the unselected background color
		int listIndex = ((BListView*)owner)->IndexOf(this);
		if(fmod(listIndex, 2))
			backgroundColor = ui_color(B_DOCUMENT_BACKGROUND_COLOR);
		else
			backgroundColor = ui_color(B_CONTROL_BACKGROUND_COLOR);
	}
	owner->SetHighColor(backgroundColor);
	owner->SetLowColor(backgroundColor);
	owner->FillRect(item_rect);
	
	//icon
	if (fIcon) {
		float offsetMarginHeight = floor( (listItemHeight - fIcon->Bounds().Height())/2);
		float offsetMarginWidth = floor( (fIconSize - fIcon->Bounds().Width())/2 ) + kIconMargin;
		owner->SetDrawingMode(B_OP_OVER);
		owner->DrawBitmap(fIcon, BPoint(item_rect.left + offsetMarginWidth,
							item_rect.top + offsetMarginHeight));
		owner->SetDrawingMode(B_OP_COPY);
		offset_width += fIconSize + 2*kIconMargin;
	}
	// no icon, give the text some margin space
	else
	{
		offset_width += kTextMargin;
	}
	
	//text
	if(listItemHeight > fTextOnlyHeight)
		offset_height += floor( (listItemHeight - fTextOnlyHeight)/2 );
			// center the text vertically
	BPoint cursor(item_rect.left + offset_width, item_rect.top + offset_height + kTextMargin);
	owner->SetHighColor(ui_color(B_CONTROL_TEXT_COLOR));
	owner->MovePenTo(cursor.x, cursor.y);
	owner->DrawString(fName);
	if(fDrawTwoLines)
		owner->MovePenTo(cursor.x, cursor.y + fFontHeight + kTextMargin);
	else
		owner->DrawString(" ");
	owner->DrawString(fStatusText);
}



void
ServiceListItem::Update(BView *owner, const BFont *font)
{
	BListItem::Update(owner, font);
	_UpdateHeight(font);
}


void
ServiceListItem::_UpdateHeight(const BFont *font)
{
	font_height fontHeight;
	font->GetHeight(&fontHeight);
	fFontHeight = fontHeight.ascent + fontHeight.descent + fontHeight.leading;
	fFontAscent = fontHeight.ascent;
	if(fDrawTwoLines)
		fTextOnlyHeight = 2*fFontHeight + 3*kTextMargin;
	else
		fTextOnlyHeight = fFontHeight + 2*kTextMargin;
	
	if(fIcon) {
		float iconSpacing = fIconSize + 2*kIconMargin;
		if(iconSpacing > fTextOnlyHeight)
			SetHeight(iconSpacing);
		else
			SetHeight(fTextOnlyHeight);
	}
	else
		SetHeight(fTextOnlyHeight);
}
