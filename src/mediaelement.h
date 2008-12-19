/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * mediaelement.h:
 *
 * Contact:
 *   Moonlight List (moonlight-list@lists.ximian.com)
 *
 * Copyright 2007 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 */

#ifndef __MOON_MEDIAELEMENT_H__
#define __MOON_MEDIAELEMENT_H__

#include <glib.h>
#include <gdk/gdkpixbuf.h>

#include "mediaplayer.h"
#include "value.h"
#include "brush.h"
#include "frameworkelement.h"
#include "error.h"
#include "pipeline.h"

/* @Namespace=None */
class MediaErrorEventArgs : public ErrorEventArgs {
 protected:
	virtual ~MediaErrorEventArgs () {}

 public:
	MediaErrorEventArgs (MediaResult result, const char *msg)
		: ErrorEventArgs (MediaError, (int) result, msg)
	{		
	}
	
	virtual Type::Kind GetObjectType () { return Type::MEDIAERROREVENTARGS; }

	MediaResult GetMediaResult () { return (MediaResult) error_code; }
};

/* @Namespace=System.Windows.Controls */
class MediaElement : public MediaBase {
 public:
	enum MediaElementState {
		Closed,
		Opening,
		Buffering,
		Playing,
		Paused,
		Stopped,
		Error,
	};
	
 private:
	static void data_write (void *data, gint32 offset, gint32 n, void *closure);
	static void data_request_position (gint64 *pos, void *closure);
	static void size_notify (gint64 size, gpointer data);
	
	TimelineMarkerCollection *streamed_markers;
	Queue *pending_streamed_markers;
	MediaClosure *marker_closure;
	int advance_frame_timeout_id;
	bool recalculate_matrix;
	cairo_matrix_t matrix;
	MediaPlayer *mplayer;
	Playlist *playlist;
	Media *media;
	
	// When checking if a marker has been reached, we need to 
	// know the last time the check was made, to see if 
	// the marker's pts hit the region.
	guint64 previous_position;
	// When the position is changed by the client, we store the requested position
	// here and do the actual seeking async. Note that we might get several seek requests
	// before the actual seek is done, currently we just seek to the last position requested,
	// the previous requests are ignored. -1 denotes that there are no pending seeks.
	TimeSpan seek_to_position;
	
	// Buffering can be caused by:
	//   [1] When the media is opened, we automatically buffer an amount equal to BufferingTime.
	//     - In this case we ask the pipeline how much it has buffered.
	//
	//   [2] When during playback we realize that we don't have enough data.
	//     - In this case we ask the pipelien how much it has buffered.
	//
	//   [3] When we seek, and realize that we don't have enough data.
	//     - In this case the buffering progress is calculated as:
	//       ("last available pts" - "last played pts") / ("seeked to pts" - "last played pts" + BufferingTime)
	//
	guint64 last_played_pts;
	guint64 first_pts; // the first pts, starts off at GUINT_MAX
	int buffering_mode; // if we're in [3] or not: 0 = unknown, 1 = [1], etc.
	
	// this is used to know what to do after a Buffering state finishes
	MediaElementState prev_state;
	
	// The current state of the media element.
	MediaElementState state;
	
	guint32 flags;
	
	// downloader methods/data
	IMediaSource *downloaded_file;
	
	void DataWrite (void *data, gint32 offset, gint32 n);
	void DataRequestPosition (gint64 *pos);
	virtual void DownloaderComplete ();
	virtual void DownloaderFailed (EventArgs *args);
	void BufferingComplete ();
	double GetBufferedSize ();
	double CalculateBufferingProgress ();
	void UpdateProgress ();
	
	virtual void OnLoaded ();
	
	// Try to open the media (i.e. read the headers).
	void TryOpen ();
	
	// Checks if the media was actually a playlist, in which case false is returned.
	// Fill in all information from the opened media and raise MediaOpenedEvent. Does not change any state.
	bool MediaOpened (Media *media);
	void EmitMediaEnded ();
	
	void CheckMarkers (guint64 from, guint64 to, TimelineMarkerCollection *col, bool remove);
	void CheckMarkers (guint64 from, guint64 to);
	void ReadMarkers ();
	
	TimeSpan UpdatePlayerPosition (TimeSpan position);
	
	//
	// Private Property Accessors
	//
	void SetAudioStreamCount (int count);
	
	void SetBufferingProgress (double progress);
	
	void SetCanPause (bool set);
	void SetCanSeek (bool set);

	// XXX NaturalDurationProperty only generates a getter because
	// this setter doesn't take a Duration.  why the disconnect?
	void SetNaturalDuration (TimeSpan duration);

	void SetNaturalVideoHeight (double height);
	void SetNaturalVideoWidth (double width);
	
	void PlayOrStopNow ();
	void PauseNow ();
	void PlayNow ();
	void StopNow ();
	void SeekNow ();
	static void PauseNow (EventObject *value);
	static void PlayNow (EventObject *value);
	static void StopNow (EventObject *value);
	static void SeekNow (EventObject *value);
	
 protected:
	virtual DownloaderAccessPolicy GetDownloaderPolicy (const char *uri);
	virtual ~MediaElement ();
	
 public:
	// properties
 	/* @PropertyType=MediaAttributeCollection,ManagedPropertyType=Dictionary<string\,string>,ManagedSetterAccess=Internal,GenerateAccessors,Validator=MediaAttributeCollectionValidator */
	static DependencyProperty *AttributesProperty;
 	/* @PropertyType=gint32,DefaultValue=0,ReadOnly,GenerateAccessors */
	static DependencyProperty *AudioStreamCountProperty;
 	/* @PropertyType=gint32,Nullable,GenerateAccessors */
	static DependencyProperty *AudioStreamIndexProperty;
 	/* @PropertyType=bool,DefaultValue=true,GenerateAccessors */
	static DependencyProperty *AutoPlayProperty;
 	/* @PropertyType=double,DefaultValue=0.0,GenerateAccessors */
	static DependencyProperty *BalanceProperty;
 	/* @PropertyType=double,DefaultValue=0.0,ReadOnly,GenerateAccessors */
	static DependencyProperty *BufferingProgressProperty;
 	/* @PropertyType=TimeSpan,DefaultValue="TimeSpan_FromSeconds (5)\,Type::TIMESPAN",GenerateAccessors */
	static DependencyProperty *BufferingTimeProperty;
 	/* @PropertyType=bool,DefaultValue=false,ReadOnly,GenerateAccessors */
	static DependencyProperty *CanPauseProperty;
 	/* @PropertyType=bool,DefaultValue=false,ReadOnly,GenerateAccessors */
	static DependencyProperty *CanSeekProperty;
 	/* @PropertyType=string,ReadOnly,ManagedPropertyType=MediaElementState,GenerateAccessors */
	static DependencyProperty *CurrentStateProperty;
 	/* @PropertyType=bool,DefaultValue=false,GenerateAccessors */
	static DependencyProperty *IsMutedProperty;
 	/* @PropertyType=TimelineMarkerCollection,ManagedFieldAccess=Internal,ManagedSetterAccess=Internal,GenerateAccessors */
	static DependencyProperty *MarkersProperty;
 	/* @PropertyType=Duration,DefaultValue=Duration::FromSeconds (0),ReadOnly,GenerateGetter */
	static DependencyProperty *NaturalDurationProperty;
 	/* @PropertyType=double,DefaultValue=0.0,ReadOnly,ManagedPropertyType=int,GenerateAccessors,Validator=IntGreaterThanZeroValidator */
	static DependencyProperty *NaturalVideoHeightProperty;
 	/* @PropertyType=double,DefaultValue=0.0,ReadOnly,ManagedPropertyType=int,GenerateAccessors,Validator=IntGreaterThanZeroValidator */
	static DependencyProperty *NaturalVideoWidthProperty;
 	/* @PropertyType=TimeSpan,GenerateAccessors */
	static DependencyProperty *PositionProperty;
 	/* @PropertyType=double,DefaultValue=0.5,GenerateAccessors */
	static DependencyProperty *VolumeProperty;

 	/* @PropertyType=double,DefaultValue=0.0,GenerateAccessors,ReadOnly */
	static DependencyProperty *DownloadProgressOffsetProperty;
	/* @PropertyType=double,DefaultValue=0.0,GenerateAccessors,ReadOnly */
	static DependencyProperty *DroppedFramesPerSecondProperty;
	/* @PropertyType=double,DefaultValue=0.0,GenerateAccessors,ReadOnly */
	static DependencyProperty *RenderedFramesPerSecondProperty;
	
	// events
	const static int BufferingProgressChangedEvent;
	const static int CurrentStateChangedEvent;
	const static int MarkerReachedEvent;
	const static int MediaEndedEvent;
	const static int MediaFailedEvent;
	// MediaOpened is raised when media is ready to play (we've already started playing, or, if AutoPlay is false, paused).
	const static int MediaOpenedEvent;
	
 	/* @GenerateCBinding,GeneratePInvoke */
	MediaElement ();
	virtual Type::Kind GetObjectType () { return Type::MEDIAELEMENT; }
	
	virtual void SetSurface (Surface *surface);
	
	bool AdvanceFrame ();
	// Called by MediaPlayer when the media reaches its end.
	// For media with both video and audio this method is called
	// after both have finished.
	void MediaFinished ();
	
	MediaPlayer *GetMediaPlayer () { return mplayer; }
	
	// overrides
	virtual void Render (cairo_t *cr, Region *region);
	virtual Point GetTransformOrigin ();
	virtual Rect GetCoverageBounds ();
	
	virtual Value *GetValue (DependencyProperty *prop);
	virtual void OnPropertyChanged (PropertyChangedEventArgs *args);
	
	virtual void SetSourceInternal (Downloader *downloader, char *PartName);
	virtual void SetSource (Downloader *downloader, const char *PartName);
	
	/* @GenerateCBinding,GeneratePInvoke,Version=2.0 */
	void SetStreamSource (ManagedStreamCallbacks *stream);
	
	/* @GenerateCBinding,GeneratePInvoke */
	void Pause ();
	
	/* @GenerateCBinding,GeneratePInvoke */
	void Play ();
	
	/* @GenerateCBinding,GeneratePInvoke */
	void Stop ();
	
	// These methods are the ones that actually call the appropiate methods on the MediaPlayer
	void PlayInternal ();
	//void PauseInternal ();
	//void StopInternal ();
	
	// State methods
	bool IsClosed () { return state == Closed; }
	bool IsOpening () { return state == Opening; }
	bool IsBuffering () { return state == Buffering; }
	bool IsPlaying () { return state == Playing; }
	bool IsPaused () { return state == Paused; }
	bool IsStopped () { return state == Stopped; }
	
	bool IsLive ();
	bool IsMissingCodecs ();
	void EmitMediaOpened ();
	void Reinitialize (bool dtor); // dtor is true if we're calling from the destructor.
	
	pthread_mutex_t open_mutex; // Used when accessing closure.
	MediaClosure *closure;
	static void TryOpenFinished (EventObject *user_data);
	void SetPlayRequested ();
	
	// Reset all information to defaults, set state to 'Error' and raise MediaFailedEvent
	void MediaFailed (ErrorEventArgs *args = NULL);
	
	static const char *GetStateName (MediaElementState state);
	
	MediaElementState GetState () { return state; }
	void SetState (MediaElementState state);
	
	Playlist *GetPlaylist () { return playlist;  }
	
	virtual bool EnableAntiAlias ();
	
	void AddStreamedMarker (TimelineMarker *marker);
	static void AddStreamedMarkersCallback (EventObject *obj);
	void AddStreamedMarkers ();
	void SetMedia (Media *media);
	
	//
	// Public Property Accessors
	//
	void SetAttributes (MediaAttributeCollection *attrs);
	MediaAttributeCollection *GetAttributes ();
	
	int GetAudioStreamCount ();
	
	void SetAudioStreamIndex (gint32 index);
	void SetAudioStreamIndex (gint32* index);
	gint32* GetAudioStreamIndex ();
	
	void SetAutoPlay (bool set);
	bool GetAutoPlay ();
	
	void SetBalance (double balance);
	double GetBalance ();
	
	double GetBufferingProgress ();
	
	void SetBufferingTime (TimeSpan time);
	TimeSpan GetBufferingTime ();
	
	bool GetCanPause ();
	bool GetCanSeek ();
	
	void SetCurrentState (const char *state);
	const char *GetCurrentState ();
	
	void SetIsMuted (bool set);
	bool GetIsMuted ();
	
	void SetMarkers (TimelineMarkerCollection *markers);
	TimelineMarkerCollection *GetMarkers ();
	
	Duration *GetNaturalDuration ();
	double GetNaturalVideoHeight ();
	double GetNaturalVideoWidth ();
	
	void SetPosition (TimeSpan position);
	TimeSpan GetPosition ();
	
	void SetVolume (double volume);
	double GetVolume ();

	void SetDownloadProgressOffset (double value);
	double GetDownloadProgressOffset ();

	void SetRenderedFramesPerSecond (double value);
	double GetRenderedFramesPerSecond ();

	void SetDroppedFramesPerSecond (double value);
	double GetDroppedFramesPerSecond ();
};

#endif /* __MEDIAELEMENT_H__ */
