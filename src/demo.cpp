#define VIDEO_DEMO
#define XAML_DEMO
#include <string.h>
#include <gtk/gtk.h>
#include <malloc.h>
#include <glib.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "runtime.h"
#include "transform.h"
#include "animation.h"
#include "shape.h"

static UIElement *v;
static Rectangle *r;

static RotateTransform *r_trans;
static RotateTransform *v_trans;
static ScaleTransform *s_trans;
static TranslateTransform *t_trans;

extern NameScope *global_NameScope;

static int do_fps = FALSE;
static uint64_t last_time;

static GtkWidget *w;

static int64_t
gettime ()
{
    struct timeval tv;

    gettimeofday (&tv, NULL);

    return (int64_t) tv.tv_sec * 1000000 + tv.tv_usec;
}

static gboolean
my_expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
}

static gboolean
invalidator (gpointer data)
{
	Surface *s = (Surface *) data;

	gtk_widget_queue_draw (s->drawing_area);

	int64_t now = gettime ();

	int64_t diff = now - last_time;
	if ((now - last_time) > 1000000){
		last_time = now;
		char *res = g_strdup_printf ("%d fps", s->frames);

		gtk_window_set_title (GTK_WINDOW (w), res);
		g_free (res);
		s->frames = 0;
	}

	return TRUE;
}

static gboolean
delete_event (GtkWidget *widget, GdkEvent *e, gpointer data)
{
	gtk_main_quit ();
	return 1;
}

int
main (int argc, char *argv [])
{
	GtkWidget *w2, *box, *button;
	cairo_matrix_t trans;
	double dash = 3.5;
	char *file = NULL;

	gtk_init (&argc, &argv);
	g_thread_init (NULL);
	gdk_threads_init ();
	runtime_init ();

	TimeManager::Instance()->Start();

	w = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect (GTK_OBJECT (w), "delete_event", G_CALLBACK (delete_event), NULL);
	Surface *t = surface_new (600, 600);
	gtk_container_add (GTK_CONTAINER(w), t->drawing_area);
		
	for (int i = 1; i < argc; i++){
		if (strcmp (argv [i], "-h") == 0){
			printf ("usage is: demo [-fps] [file.xaml]\n");
			return 0;
		} else if (strcmp (argv [i], "-fps")== 0){
			do_fps = TRUE;
		}else
			file = argv [i];
	}

	if (file){
		gtk_window_set_title (GTK_WINDOW (w), file);

		UIElement *e = xaml_create_from_file (file);
		if (e == NULL){
			printf ("Was not able to load the file\n");
			return 1;
		}

		surface_attach (t, e);
		
	} else {
		Canvas *canvas = new Canvas ();
		surface_attach (t, canvas);

		// Create our objects
		r_trans = new RotateTransform ();
		v_trans = new RotateTransform ();
		s_trans = new ScaleTransform ();
		t_trans = new TranslateTransform ();
		
		r = rectangle_new ();
		framework_element_set_width (r, 50.0);
		framework_element_set_height (r, 50.0);
		shape_set_stroke_thickness (r, 10.0);
		r->SetValue (Canvas::LeftProperty, Value (50.0));
		r->SetValue (Canvas::TopProperty, Value (50.0));
		
		rectangle_set_radius_x (r, 10);
		rectangle_set_radius_y (r, 20);
		item_set_render_transform (r, s_trans);
		Color *c = new Color (1.0, 0.0, 0.5, 0.5);
		SolidColorBrush *scb = new SolidColorBrush ();
		solid_color_brush_set_color (scb, c);
		shape_set_stroke (r, scb);
		Color *c2 = new Color (0.5, 0.5, 0.0, 0.25);
		SolidColorBrush *scb2 = new SolidColorBrush ();
		solid_color_brush_set_color (scb2, c2);
		shape_set_fill (r, scb2);
		
		Rectangle *r2 = rectangle_new ();
		framework_element_set_width (r2, 50.0);
		framework_element_set_height (r2, 50.0);
		shape_set_stroke_dash_array (r2, &dash, 1);
		r2->SetValue (Canvas::LeftProperty, Value (50.0));
		r2->SetValue (Canvas::TopProperty, Value (50.0));
		item_set_render_transform (r2, r_trans);
		shape_set_stroke (r2, scb);
		panel_child_add (canvas, r2);
		
#ifdef XAML_DEMO
		panel_child_add (canvas, xaml_create_from_str ("<Line Stroke='Blue' X1='10' Y1='10' X2='10' Y2='300' />"));
#endif
		
#ifdef VIDEO_DEMO
		v = video_new ("file:///tmp/BoxerSmacksdownInhoffe.wmv");
		item_set_render_transform (v, v_trans);
		item_set_transform_origin (v, Point (1, 1));
		printf ("Got %d\n", v);
		panel_child_add (canvas, v);
#endif
		
		panel_child_add (canvas, r);
		
#ifdef VIDEO_DEMO
		//UIElement *v2 = video_new ("file:///tmp/Countdown-Colbert-BestNailings.wmv");
		//UIElement *v2 = video_new ("file:///tmp/red.wmv", 100, 100);
		UIElement *v2 = video_new ("file:///tmp/BoxerSmacksdownInhoffe.wmv");
		v2->SetValue (Canvas::LeftProperty, Value (100.0));
		v2->SetValue (Canvas::TopProperty, Value (100.0));
		item_set_render_transform (v2, s_trans);
		panel_child_add (canvas, v2);
#endif

		Storyboard *sb = new Storyboard ();

		DoubleAnimation *r_anim = new DoubleAnimation ();
		DoubleAnimation *v_anim = new DoubleAnimation ();
		DoubleAnimation *sx_anim = new DoubleAnimation ();
		DoubleAnimation *sy_anim = new DoubleAnimation ();

		// The rectangle rotates completely around every 4
		// seconds, and stops after the second time around
		global_NameScope->RegisterName ("rect-transform", r_trans);
		double_animation_set_to (r_anim, -360.0);
		timeline_set_repeat_behavior (r_anim, RepeatBehavior(2.0));
		timeline_set_duration (r_anim, Duration::FromSeconds (4));
		timeline_group_add_child (sb, r_anim);
		storyboard_child_set_target_name (sb, r_anim, "rect-transform");
		storyboard_child_set_target_property (sb, r_anim, "Angle");

		// The rotating video takes 5 seconds to complete the rotation
		global_NameScope->RegisterName ("video-transform", v_trans);
		double_animation_set_to (v_anim, 360.0);
		timeline_set_repeat_behavior (v_anim, RepeatBehavior::Forever);
		timeline_set_duration (v_anim, Duration::FromSeconds (5));
		timeline_group_add_child (sb, v_anim);
		storyboard_child_set_target_name (sb, v_anim, "video-transform");
		storyboard_child_set_target_property (sb, v_anim, "Angle");


		// for the scaled items, we scale X and Y differently,
		// the X scaling is completed in 6 seconds, and the y
		// in 7.
		global_NameScope->RegisterName ("scale-transform", s_trans);

		double_animation_set_from (sx_anim, 1.0);
		double_animation_set_to (sx_anim, 0.0);
		timeline_set_repeat_behavior (sx_anim, RepeatBehavior::Forever);
		timeline_set_duration (sx_anim, Duration::FromSeconds (6));
		timeline_set_autoreverse (sx_anim, true);
		timeline_group_add_child (sb, sx_anim);
		storyboard_child_set_target_name (sb, sx_anim, "scale-transform");
		storyboard_child_set_target_property (sb, sx_anim, "ScaleX");

		double_animation_set_from (sy_anim, 1.0);
		double_animation_set_to (sy_anim, 0.0);
		timeline_set_repeat_behavior (sy_anim, RepeatBehavior::Forever);
		timeline_set_duration (sy_anim, Duration::FromSeconds (7));
		timeline_set_autoreverse (sy_anim, true);
		timeline_group_add_child (sb, sy_anim);
		storyboard_child_set_target_name (sb, sy_anim, "scale-transform");
		storyboard_child_set_target_property (sb, sy_anim, "ScaleY");
		sb->Begin ();
	}		
	if (do_fps){
		t->frames = 0;
		last_time = gettime ();
		gtk_idle_add (invalidator, t);
	}
	gtk_widget_set_usize (w, 600, 400);
	gtk_widget_show_all (w);
	gtk_main ();
}
