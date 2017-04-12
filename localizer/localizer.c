#include <tiams.h>
#define STRINGIFY_AUX(a) #a
#define STRINGIFY(a) STRINGIFY_AUX(a)
#define CONCAT_AUX(a, b) a##b
#define CONCAT(a, b) CONCAT_AUX(a, b)
#include STRINGIFY(CONCAT(../translations/, TRANSLATION_FILE))
#include "../botr/tr.h"
#include "../botr/string_frame_builder.h"


static void main (pFrame self, PEvent e);
static void observer (pFrame self, PEvent e);
static BOOL localize (AppID self, const char *requested_lang);
static void unlocalize (AppID self);
static void notice_install (AppID self, ACB const *pacb);
static BOOL can_delete (AppID self);


static FRAME(localizer_app_frame, OO_SYSTEM_FRAME, 0, OO_APP_FLAGS, 7)
	// This app does not appear on the [APPS] menu
	ATTR(OO_APP_FLAGS, APP_NONE)
	ATTR(OO_APP_NAME, LOCALIZER_APP_NAME)

	// It needs to respond to some events
	ATTR(OO_APP_PROCESS_EVENT, &main)

	// Export methods used in localization
	ATTR(OO_APP_LOCALIZE, &localize)
	ATTR(OO_APP_UNLOCALIZE, &unlocalize)
	ATTR(OO_APP_CAN_DELETE, &can_delete)
	ATTR(OO_APP_NOTICE_INSTALL, &notice_install)
ENDFRAME

static pFrame appframe = (pFrame)&localizer_app_frame;

// Set target_app to the internal name of the app which this localizer will hook into. This is the same name described in section 7.1.3.2. Internal Application Name.
unsigned char const target_app[] = "blocks";

/*
This little FRAME is hooked ahead of the target application's frame. It accomplishes a couple of things:
1) It renames the target application on the [APPS] menu, and
2) It redirects its event handler to this application so we can observe its pack/unpack and uninstall messages. All events are forwarded to their rightful owner.
*/
static FRAME(botr_app_frame, OO_SYSTEM_FRAME, NULL, OO_APP_NAME, 2)

	// Override the Target app's name in the [APPS] menu
	ATTR(OO_APP_NAME, BOTR_APP_NAME)

	// Redirect Target app's events to me
	ATTR(OO_APP_PROCESS_EVENT, &observer)
ENDFRAME

/*
Frame containing the translated strings
*/
static STRING_FRAME(target_strings, OO_SYSTEM_FRAME, &botr_app_frame, OO_APPSTRING, NUM_STRINGS)
	APP_STRING_LIST
ENDFRAME

// Keep track of when this localizer is hooked into the target application
static pFrame hook = 0;


/*
This app's event handler needs to unhook itself from the target application when it is deleted or moved.
It can reconnect to the target application when it has been reinstalled or has completed its move.
*/

static void main (pFrame self, PEvent e) {
	switch (e->command) {
		case CM_UNINSTALL:
		case CM_PACK:
			unlocalize (0 /*self*/);
			break;

		case CM_INSTALL:
		case CM_UNPACK:
			localize (0 /*self*/, XR_stringPtr (XR_NativeLanguage));
			break;
	}
}


/*
This routine observes all events sent to the target application.
It localizes the target after the application UNPACKs and unlocalizes the target just before it PACKs or UNINSTALLs.
*/

static void observer (pFrame self, PEvent e) {
	Access_AMS_Global_Variables;

	pFrame super = OO_SuperFrame;

	switch (e->command) {
		case CM_UNPACK:
			// Pass the event on to the target application before applying the localizer hook.
			AppProcessEvent (super, e);
			localize (0 /*self*/, XR_stringPtr (XR_NativeLanguage));
			break;

		case CM_UNINSTALL:
		case CM_PACK:
			// Unhook the localizer before passing the event on to the target application
			unlocalize (0 /*self*/);

		default:
			// Forward all events to their rightful owner
			AppProcessEvent (super, e);
	}
}


/*
If requested language matches the language we know, hook over the target application

WARNING: In the example from the manual the first parameter 'self' was of type pFrame (4 bytes) -- the correct type is AppID (2 bytes)
*/

static BOOL localize (AppID self, const char *requested_lang) {
	if (hook == 0 && strcmp (requested_lang, BOTR_LANGUAGE) == 0) {
		return OO_InstallAppHookByName (target_app, (pFrame)&target_strings, &hook);
	}

	return FALSE;
}


/*
Unhook from the target application if we have anything to unhook

WARNING: In the example from the manual the parameter 'self' was of type pFrame (4 bytes) -- the correct type is AppID (2 bytes)
*/

static void unlocalize (AppID self) {
	if (hook) {
		OO_UninstallAppHookByName (target_app, hook);
		hook = 0;
	}
}


/*
We have just been notified that a new application was installed. Is it our target application? If so, apply our localization to it.

WARNING: In the example from the manual the parameter 'self' was of type pFrame (4 bytes) -- the correct type is AppID (2 bytes)
*/

static void notice_install (AppID self, ACB const *pacb) {
	if (strcmp ((const char *)pacb->appHeader->name, (const char *)target_app) == 0) {
		localize (0 /*self*/, XR_stringPtr (XR_NativeLanguage));
	}
}


/*
This application can be deleted if the target application does not exist or is not running

WARNING: In the example from the manual the parameter 'self' was of type pFrame (4 bytes) -- the correct type is AppID (2 bytes)
*/

static BOOL can_delete (AppID self) {
	Access_AMS_Global_Variables;

	AppID targetID = EV_getAppID (target_app);

	return targetID == H_NULL || (targetID != EV_appA && targetID != EV_appB);
}

