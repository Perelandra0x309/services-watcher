/* ServiceListItem.h
 * Copyright 2010 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#ifndef SERVICE_LISTITEM_H
#define SERVICE_LISTITEM_H

#include <InterfaceKit.h>
#include <StorageKit.h>
#include <Roster.h>
#include "IconUtils.h"
#include "constants.h"


enum {
	STATE_NONE = 0,
	STATE_OK_RUNNING,
	STATE_OK_STOPPING,
	STATE_OK_STOPPED,
	STATE_OK_STARTING,
	STATE_OK_RESTARTING,
	STATE_ERROR_STOPPING__IS_RUNNING,
	STATE_ERROR_STOPPING__IS_NOT_RUNNING,
	STATE_ERROR_STARTING__IS_RUNNING,
	STATE_ERROR_STARTING__IS_NOT_RUNNING
};

class ServiceListItem : public BListItem
{
public:
							ServiceListItem(BEntry entry, const char * sig, AppOptions *options);
							~ServiceListItem();
	virtual void			DrawItem(BView *owner, BRect item_rect, bool complete = false);
	virtual void			Update(BView *owner, const BFont *font);
			void			ProcessMessage(BMessage*);
	const	char*			GetSignature() { return fSignature.String(); }
			int				GetState() { return fCurrentState; }
			void			SetRunningState();
			void			SetDrawTwoLines(bool value);
			void			SetIconSize(int value);
			status_t		InitStatus() { return fInitStatus; }
private:
	status_t				fInitStatus;
	entry_ref				fEntryRef;
	BString					fSignature, fName, fStatusText;
	float					fFontHeight, fFontAscent, fTextOnlyHeight;
	BBitmap					*fIcon;
	int						fCurrentState;
	bool					fDrawTwoLines;
	int						fIconSize;
	void					_GetIcon();
	status_t				_StopService();
	status_t				_StartService();
	void					_SetStatusText(const char*);
	void					_SetState(int);
	void					_UpdateHeight(const BFont*);

};

#endif
