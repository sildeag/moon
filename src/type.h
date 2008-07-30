/*
 * Automatically generated from type.h.in, do not edit this file directly
 * To regenerate execute typegen.sh
 */
/*
 * type.h: Generated code for the type system.
 *
 * Contact:
 *   Moonlight List (moonlight-list@lists.ximian.com)
 *
 * Copyright 2007 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 * 
 */
#ifndef __TYPE_H__
#define __TYPE_H__

#include <glib.h>

class DependencyObject;
class Surface;

typedef gint64 TimeSpan;
typedef DependencyObject *create_inst_func (void);

class Type {
public:
	enum Kind {
	// START_MANAGED_MAPPING
		INVALID,
		ANIMATION,
		ANIMATIONCLOCK,
		APPLICATION,// Silverlight 2.0 only
		ARCSEGMENT,
		ASSEMBLYPART,// Silverlight 2.0 only
		ASSEMBLYPART_COLLECTION,// Silverlight 2.0 only
		BEGINSTORYBOARD,
		BEZIERSEGMENT,
		BOOL,
		BRUSH,
		CANVAS,
		CLOCK,
		CLOCKGROUP,
		COLLECTION,
		COLOR,
		COLORANIMATION,
		COLORANIMATIONUSINGKEYFRAMES,
		COLORKEYFRAME,
		COLORKEYFRAME_COLLECTION,
		COLUMNDEFINITION,// Silverlight 2.0 only
		COLUMNDEFINITION_COLLECTION,// Silverlight 2.0 only
		CONTROL,
		DEPENDENCY_OBJECT,
		DEPENDENCY_OBJECT_COLLECTION,
		DEPLOYMENT,// Silverlight 2.0 only
		DISCRETECOLORKEYFRAME,
		DISCRETEDOUBLEKEYFRAME,
		DISCRETEPOINTKEYFRAME,
		DOUBLE,
		DOUBLE_ARRAY,
		DOUBLE_COLLECTION,
		DOUBLEANIMATION,
		DOUBLEANIMATIONUSINGKEYFRAMES,
		DOUBLEKEYFRAME,
		DOUBLEKEYFRAME_COLLECTION,
		DOWNLOADER,
		DRAWINGATTRIBUTES,
		DURATION,
		ELLIPSE,
		ELLIPSEGEOMETRY,
		ERROREVENTARGS,
		EVENTARGS,
		EVENTOBJECT,
		EVENTTRIGGER,
		FRAMEWORKELEMENT,
		GEOMETRY,
		GEOMETRY_COLLECTION,
		GEOMETRYGROUP,
		GLYPHS,
		GRADIENTBRUSH,
		GRADIENTSTOP,
		GRADIENTSTOP_COLLECTION,
		GRID,// Silverlight 2.0 only
		GRIDLENGTH,
		IMAGE,
		IMAGEBRUSH,
		IMAGEERROREVENTARGS,
		INKPRESENTER,
		INLINE,
		INLINES,
		INT32,
		INT64,
		KEYBOARDEVENTARGS,
		KEYFRAME,
		KEYFRAME_COLLECTION,
		KEYSPLINE,
		KEYTIME,
		LINE,
		LINEARCOLORKEYFRAME,
		LINEARDOUBLEKEYFRAME,
		LINEARGRADIENTBRUSH,
		LINEARPOINTKEYFRAME,
		LINEBREAK,
		LINEGEOMETRY,
		LINESEGMENT,
		MANAGED,// Silverlight 2.0 only
		MANUALTIMESOURCE,
		MARKERREACHEDEVENTARGS,
		MATRIX,
		MATRIXTRANSFORM,
		MEDIAATTRIBUTE,
		MEDIAATTRIBUTE_COLLECTION,
		MEDIABASE,
		MEDIAELEMENT,
		MEDIAERROREVENTARGS,
		MOUSEEVENTARGS,
		NAMESCOPE,
		NPOBJ,
		PANEL,
		PARALLELTIMELINE,
		PARSERERROREVENTARGS,
		PATH,
		PATHFIGURE,
		PATHFIGURE_COLLECTION,
		PATHGEOMETRY,
		PATHSEGMENT,
		PATHSEGMENT_COLLECTION,
		POINT,
		POINT_ARRAY,
		POINT_COLLECTION,
		POINTANIMATION,
		POINTANIMATIONUSINGKEYFRAMES,
		POINTKEYFRAME,
		POINTKEYFRAME_COLLECTION,
		POLYBEZIERSEGMENT,
		POLYGON,
		POLYLINE,
		POLYLINESEGMENT,
		POLYQUADRATICBEZIERSEGMENT,
		QUADRATICBEZIERSEGMENT,
		RADIALGRADIENTBRUSH,
		RECT,
		RECTANGLE,
		RECTANGLEGEOMETRY,
		REPEATBEHAVIOR,
		RESOURCE_DICTIONARY,
		ROTATETRANSFORM,
		ROUTEDEVENTARGS,
		ROWDEFINITION,// Silverlight 2.0 only
		ROWDEFINITION_COLLECTION,// Silverlight 2.0 only
		RUN,
		SCALETRANSFORM,
		SHAPE,
		SIZE,// Silverlight 2.0 only
		SKEWTRANSFORM,
		SOLIDCOLORBRUSH,
		SPLINECOLORKEYFRAME,
		SPLINEDOUBLEKEYFRAME,
		SPLINEPOINTKEYFRAME,
		STORYBOARD,
		STRING,
		STROKE,
		STROKE_COLLECTION,
		STYLUSINFO,
		STYLUSPOINT,
		STYLUSPOINT_COLLECTION,
		SURFACE,
		SYSTEMTIMESOURCE,
		TEXTBLOCK,
		THICKNESS,
		TILEBRUSH,
		TIMELINE,
		TIMELINE_COLLECTION,
		TIMELINEGROUP,
		TIMELINEMARKER,
		TIMELINEMARKER_COLLECTION,
		TIMEMANAGER,
		TIMESOURCE,
		TIMESPAN,
		TRANSFORM,
		TRANSFORM_COLLECTION,
		TRANSFORMGROUP,
		TRANSLATETRANSFORM,
		TRIGGER_COLLECTION,
		TRIGGERACTION,
		TRIGGERACTION_COLLECTION,
		UIELEMENT,
		UINT32,
		UINT64,
		USERCONTROL,// Silverlight 2.0 only
		VIDEOBRUSH,
		VISUAL,
		VISUAL_COLLECTION,
		VISUALBRUSH,

		LASTTYPE,
	// END_MANAGED_MAPPING
		};

	static Type* Find (const char *name);
	static Type* Find (Type::Kind type);
	static Type* Find (Surface *surface, Type::Kind type);
	
	bool IsSubclassOf (Type::Kind super);
	static bool IsSubclassOf (Type::Kind type, Type::Kind super);

	int LookupEvent (const char *event_name);
	const char *LookupEventName (int id);
	DependencyObject *CreateInstance ();
	const char *GetContentPropertyName ();

	Type::Kind GetKind () { return type; }
	Type::Kind GetParent () { return parent; }
	bool IsValueType () { return value_type; }
	const char *GetName () { return name; }
	int GetEventCount () { return total_event_count; }

public: // private:
	Type::Kind type; // this type
	Type::Kind parent; // parent type, INVALID if no parent
	bool value_type; // if this type is a value type

	const char *name; // The name as it appears in code.
	const char *kindname; // The name as it appears in the Type::Kind enum.

	int event_count; // number of events in this class
	int total_event_count; // number of events in this class and all base classes
	const char **events; // the events this class has

	create_inst_func *create_inst; // a function pointer to create an instance of this type

	const char *content_property;
};

extern Type type_infos [Type::LASTTYPE + 1];

G_BEGIN_DECLS

bool type_get_value_type (Type::Kind type);
DependencyObject *type_create_instance (Type *type);
DependencyObject *type_create_instance_from_kind (Type::Kind kind);

void types_init (void);
const char *type_get_name (Type::Kind type);
bool type_is_dependency_object (Type::Kind type);

G_END_DECLS

#endif

