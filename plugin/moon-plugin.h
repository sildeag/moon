/*
 * moon-plugin.h: MoonLight browser plugin.
 *
 * Author:
 *   Everaldo Canuto (everaldo@novell.com)
 *
 * Copyright 2007 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 * 
 */

#ifndef MOON_PLUGIN
#define MOON_PLUGIN

#include <stdio.h>
#include <string.h>
#include <Xlib.h>

#include "npapi.h"
#include "npupp.h"

#include "glib.h"
#include "gtk/gtk.h"

#include "runtime.h"

#define DEBUG_PLUGIN

#define PLUGIN_NAME         "Novell MoonLight"
#define PLUGIN_DESCRIPTION  "Novell MoonLight is a Free Unix implementation of SilverLight";
#define PLUGIN_VERSION      "0.1"
#define MIME_TYPES_HANDLED  "application/ag-plugin:scr:WPFe Plug-In"

#ifdef DEBUG_PLUGIN
#ifdef G_LOG_DOMAIN
#undef G_LOG_DOMAIN
#endif
#define G_LOG_DOMAIN "MoonLight"
#define DEBUG(x...) g_message (x)
#else
#define DEBUG(msg)
#endif

class PluginInstance {
 private:
	void CreateControls ();

  	uint16 mode;           // NP_EMBED, NP_FULL, or NP_BACKGROUND
	NPWindow *window;      // Mozilla window object
	NPP instance;          // Mozilla instance object
	bool xembed_supported; // XEmbed Extension supported

 public:	
	PluginInstance (NPP instance, uint16 mode);
	~PluginInstance ();

	NPError GetValue (NPPVariable variable, void *result);
	NPError SetWindow (NPWindow* window);
	NPError NewStream (NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype);
	void StreamAsFile (NPStream* stream, const char* fname);
	NPError DestroyStream (NPStream* stream, NPError reason);
	int32 WriteReady (NPStream* stream);
	int32 Write (NPStream* stream, int32 offset, int32 len, void* buffer);
	void UrlNotify (const char* url, NPReason reason, void* notifyData);
	void Print (NPPrint* platformPrint);
	int16 EventHandle (void* event);

	GtkWidget *container;  // plugin container object
	GtkWidget *canvas;     // plugin canvas object
 	Surface *surface;      // plugin surface object
};

#endif /* MOON_PLUGIN */
