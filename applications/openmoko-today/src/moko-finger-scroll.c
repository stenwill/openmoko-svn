
#include "moko-finger-scroll.h"

G_DEFINE_TYPE (MokoFingerScroll, moko_finger_scroll, GTK_TYPE_EVENT_BOX)
#define FINGER_SCROLL_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), MOKO_TYPE_FINGER_SCROLL, MokoFingerScrollPrivate))
typedef struct _MokoFingerScrollPrivate MokoFingerScrollPrivate;

struct _MokoFingerScrollPrivate {
	MokoFingerScrollMode mode;
	gdouble x;
	gdouble y;
	gdouble ex;
	gdouble ey;
	gboolean enabled;
	gboolean clicked;
	gboolean moved;
	GTimeVal click_start;
	gdouble vmin;
	gdouble vmax;
	gdouble decel;
	guint sps;
	gdouble vel_x;
	gdouble vel_y;
};

enum {
	PROP_ENABLED = 1,
	PROP_MODE,
	PROP_VELOCITY_MIN,
	PROP_VELOCITY_MAX,
	PROP_DECELERATION,
	PROP_SPS,
};

static gboolean
moko_finger_scroll_button_press_cb (MokoFingerScroll *scroll,
				    GdkEventButton *event,
				    gpointer user_data)
{
	MokoFingerScrollPrivate *priv = FINGER_SCROLL_PRIVATE (scroll);

	if ((!priv->enabled) || (priv->clicked) || (event->button != 1))
		return FALSE;

	g_get_current_time (&priv->click_start);
	priv->x = event->x;
	priv->y = event->y;
	priv->moved = FALSE;
	priv->clicked = TRUE;
	/* Set velocity to zero so as to stop possible scrolling in progress */
	priv->vel_x = 0;
	priv->vel_y = 0;
	
	return TRUE;
}

static void
moko_finger_scroll_scroll (MokoFingerScroll *scroll, gdouble x, gdouble y,
			   gboolean *sx, gboolean *sy)
{
	/* Scroll by a particular amount (in pixels). Optionally, return if
	 * the scroll on a particular axis was successful.
	 */
	gdouble h, v;
	GtkAdjustment *hadjust, *vadjust;
	
	if (!GTK_BIN (scroll)->child) return;
	
	g_object_get (G_OBJECT (GTK_BIN (scroll)->child), "hadjustment",
		&hadjust, "vadjustment", &vadjust, NULL);
	
	if (sx) *sx = TRUE;
	if (sy) *sy = TRUE;
	
	if (hadjust) {
		h = gtk_adjustment_get_value (hadjust) - x;
		if (h > hadjust->upper - hadjust->page_size) {
			if (sx) *sx = FALSE;
			h = hadjust->upper - hadjust->page_size;
		} else if (h < hadjust->lower) {
			if (sx) *sx = FALSE;
			h = hadjust->lower;
		}
		gtk_adjustment_set_value (hadjust, h);
	}
	
	if (vadjust) {
		v = gtk_adjustment_get_value (vadjust) - y;
		if (v > vadjust->upper - vadjust->page_size) {
			if (sy) *sy = FALSE;
			v = vadjust->upper - vadjust->page_size;
		} else if (v < vadjust->lower) {
			if (sy) *sy = FALSE;
			v = vadjust->lower;
		}
		gtk_adjustment_set_value (vadjust, v);
	}
}

static gboolean
moko_finger_scroll_timeout (MokoFingerScroll *scroll)
{
	gboolean sx, sy;
	MokoFingerScrollPrivate *priv = FINGER_SCROLL_PRIVATE (scroll);
	
	if ((!priv->enabled) ||
	    (priv->mode != MOKO_FINGER_SCROLL_MODE_ACCEL)) return FALSE;
	if (!priv->clicked) {
		/* Decelerate gradually when pointer is raised */
		priv->vel_x *= priv->decel;
		priv->vel_y *= priv->decel;
		if ((ABS (priv->vel_x) < 1.0) && (ABS (priv->vel_y) < 1.0))
			return FALSE;
	}
	
	moko_finger_scroll_scroll (scroll, priv->vel_x, priv->vel_y, &sx, &sy);
	/* If the scroll on a particular axis wasn't succesful, reset the
	 * initial scroll position to the new mouse co-ordinate. This means
	 * when you get to the top of the page, dragging down won't do nothing.
	 */
	if (!sx) priv->x = priv->ex;
	if (!sy) priv->y = priv->ey;
	
	return TRUE;
}

static gboolean
moko_finger_scroll_motion_notify_cb (MokoFingerScroll *scroll,
				     GdkEventMotion *event,
				     gpointer user_data)
{
	MokoFingerScrollPrivate *priv = FINGER_SCROLL_PRIVATE (scroll);
	gint dnd_threshold;
	gdouble x, y;

	if ((!priv->enabled) || (!priv->clicked)) return FALSE;

	/* Only start the scroll if the mouse cursor passes beyond the
	 * DnD threshold for dragging.
	 */
	g_object_get (G_OBJECT (gtk_settings_get_default ()),
		"gtk-dnd-drag-threshold", &dnd_threshold, NULL);
	x = event->x - priv->x;
	y = event->y - priv->y;
	
	if ((!priv->moved) && (
	     (ABS (x) > dnd_threshold) || (ABS (y) > dnd_threshold))) {
		priv->moved = TRUE;
		if (priv->mode == MOKO_FINGER_SCROLL_MODE_ACCEL) {
			g_timeout_add (priv->sps,
				(GSourceFunc)moko_finger_scroll_timeout,
				scroll);
		}
	}
	
	if (priv->moved) {
		switch (priv->mode) {
		    case MOKO_FINGER_SCROLL_MODE_PUSH :
			/* Scroll by the amount of pixels the cursor has moved
			 * since the last motion event.
			 */
			moko_finger_scroll_scroll (scroll, x, y, NULL, NULL);
			priv->x = event->x;
			priv->y = event->y;
			break;
		    case MOKO_FINGER_SCROLL_MODE_ACCEL :
			/* Set acceleration relative to the initial click */
			priv->ex = event->x;
			priv->ey = event->y;
			priv->vel_x = ((x > 0) ? 1 : -1) *
				(((ABS (x) /
				 (gdouble)GTK_WIDGET (scroll)->
				 allocation.width) *
				(priv->vmax-priv->vmin)) + priv->vmin);
			priv->vel_y = ((y > 0) ? 1 : -1) *
				(((ABS (y) /
				 (gdouble)GTK_WIDGET (scroll)->
				 allocation.height) *
				(priv->vmax-priv->vmin)) + priv->vmin);
			break;
		    default :
			break;
		}
	}
	
	return TRUE;
}

static GdkWindow *
moko_finger_scroll_get_topmost (GdkWindow *window, gint x, gint y,
				gint *x2, gint *y2)
{
	/* Find the GdkWindow at the given point, by recursing from a given
	 * parent GdkWindow. Optionally return the co-ordinates transformed
	 * relative to the child window.
	 */
	while (window) {
		GList *c, *children = gdk_window_peek_children (window);
		GdkWindow *old_window = window;
		for (c = children; c; c = c->next) {
			GdkWindow *child = (GdkWindow *)c->data;
			gint width, height, wx, wy;
			
			gdk_drawable_get_size (child, &width, &height);
			gdk_window_get_position (child, &wx, &wy);
			
			if ((x >= wx) && (x < (wx + width)) &&
			    (y >= wy) && (y < (wy + height))) {
				x -= wx; y -= wy;
				window = child;
				break;
			}
		}
		if (window == old_window) break;
	}
	
	if (x2) *x2 = x;
	if (y2) *y2 = y;

	return window;
}

static gboolean
moko_finger_scroll_button_release_cb (MokoFingerScroll *scroll,
				      GdkEventButton *event,
				      gpointer user_data)
{
	MokoFingerScrollPrivate *priv = FINGER_SCROLL_PRIVATE (scroll);
	GTimeVal current;
		
	if ((!priv->enabled) || (event->button != 1))
		return FALSE;

	g_get_current_time (&current);

	if ((!priv->moved) &&
	    ((current.tv_sec > priv->click_start.tv_sec) ||
	    (current.tv_usec - priv->click_start.tv_usec > 500))) {
		GtkAdjustment *hadjust, *vadjust;
		gint x, y;
		GdkWindow *child;
		
		g_object_get (G_OBJECT (GTK_BIN (scroll)->child), "hadjustment",
			&hadjust, "vadjustment", &vadjust, NULL);

		child = moko_finger_scroll_get_topmost (
			GTK_BIN (scroll)->child->window,
			event->x, event->y, &x, &y);

		event->x -= x;
		event->y -= y;
		
		/* Send synthetic click event */
		((GdkEvent *)event)->any.window = g_object_ref (child);
		((GdkEvent *)event)->type = GDK_BUTTON_PRESS;
		gdk_event_put ((GdkEvent *)event);
		((GdkEvent *)event)->any.window = g_object_ref (child);
		((GdkEvent *)event)->type = GDK_BUTTON_RELEASE;
		gdk_event_put ((GdkEvent *)event);
	}
	priv->clicked = FALSE;

	return TRUE;
}

static void
moko_finger_scroll_add (GtkContainer *container,
			GtkWidget    *child)
{
	GTK_CONTAINER_CLASS (moko_finger_scroll_parent_class)->add (
		container, child);
}

static void
moko_finger_scroll_remove (GtkContainer *container,
			   GtkWidget    *child)
{
	GTK_CONTAINER_CLASS (moko_finger_scroll_parent_class)->remove (
		container, child);
}

static void
moko_finger_scroll_get_property (GObject * object, guint property_id,
				 GValue * value, GParamSpec * pspec)
{
	MokoFingerScrollPrivate *priv = FINGER_SCROLL_PRIVATE (object);

	switch (property_id) {
	    case PROP_ENABLED :
		g_value_set_boolean (value, priv->enabled);
		break;
	    case PROP_MODE :
		g_value_set_int (value, priv->mode);
		break;
	    case PROP_VELOCITY_MIN :
		g_value_set_double (value, priv->vmin);
		break;
	    case PROP_VELOCITY_MAX :
		g_value_set_double (value, priv->vmax);
		break;
	    case PROP_DECELERATION :
		g_value_set_double (value, priv->decel);
		break;
	    case PROP_SPS :
		g_value_set_uint (value, priv->sps);
		break;
	    default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
moko_finger_scroll_set_property (GObject * object, guint property_id,
				 const GValue * value, GParamSpec * pspec)
{
	MokoFingerScrollPrivate *priv = FINGER_SCROLL_PRIVATE (object);

	switch (property_id) {
	    case PROP_ENABLED :
		priv->enabled = g_value_get_boolean (value);
		break;
	    case PROP_MODE :
		priv->mode = g_value_get_int (value);
		break;
	    case PROP_VELOCITY_MIN :
		priv->vmin = g_value_get_double (value);
		break;
	    case PROP_VELOCITY_MAX :
		priv->vmax = g_value_get_double (value);
		break;
	    case PROP_DECELERATION :
		priv->decel = g_value_get_double (value);
		break;
	    case PROP_SPS :
		priv->sps = g_value_get_uint (value);
		break;
	    default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
moko_finger_scroll_dispose (GObject * object)
{
	if (G_OBJECT_CLASS (moko_finger_scroll_parent_class)->dispose)
		G_OBJECT_CLASS (moko_finger_scroll_parent_class)->
			dispose (object);
}

static void
moko_finger_scroll_finalize (GObject * object)
{
	G_OBJECT_CLASS (moko_finger_scroll_parent_class)->finalize (object);
}

static void
moko_finger_scroll_class_init (MokoFingerScrollClass * klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

	g_type_class_add_private (klass, sizeof (MokoFingerScrollPrivate));

	object_class->get_property = moko_finger_scroll_get_property;
	object_class->set_property = moko_finger_scroll_set_property;
	object_class->dispose = moko_finger_scroll_dispose;
	object_class->finalize = moko_finger_scroll_finalize;
	
	container_class->add = moko_finger_scroll_add;
	container_class->remove = moko_finger_scroll_remove;

	g_object_class_install_property (
		object_class,
		PROP_ENABLED,
		g_param_spec_boolean (
			"enabled",
			"Enabled",
			"Enable or disable finger-scroll.",
			TRUE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property (
		object_class,
		PROP_MODE,
		g_param_spec_int (
			"mode",
			"Scroll mode",
			"Change the finger-scrolling mode.",
			MOKO_FINGER_SCROLL_MODE_PUSH,
			MOKO_FINGER_SCROLL_MODE_ACCEL,
			MOKO_FINGER_SCROLL_MODE_ACCEL,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property (
		object_class,
		PROP_VELOCITY_MIN,
		g_param_spec_double (
			"velocity_min",
			"Minimum scroll velocity",
			"Minimum distance the child widget should scroll "
				"per 'frame', in pixels.",
			0, G_MAXDOUBLE, 0,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property (
		object_class,
		PROP_VELOCITY_MAX,
		g_param_spec_double (
			"velocity_max",
			"Maximum scroll velocity",
			"Minimum distance the child widget should scroll "
				"per 'frame', in pixels.",
			0, G_MAXDOUBLE, 24,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property (
		object_class,
		PROP_DECELERATION,
		g_param_spec_double (
			"deceleration",
			"Deceleration multiplier",
			"The multiplier used when decelerating when in "
				"acceleration scrolling mode.",
			0, 1.0, 0.9,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	
	g_object_class_install_property (
		object_class,
		PROP_SPS,
		g_param_spec_uint (
			"sps",
			"Scrolls per second",
			"Amount of scroll events to generate per second.",
			0, G_MAXUINT, 15,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
moko_finger_scroll_init (MokoFingerScroll * self)
{
	MokoFingerScrollPrivate *priv = FINGER_SCROLL_PRIVATE (self);
	
	priv->x = 0;
	priv->y = 0;
	priv->moved = FALSE;
	priv->clicked = FALSE;	

	gtk_event_box_set_above_child (GTK_EVENT_BOX (self), TRUE);
	gtk_event_box_set_visible_window (GTK_EVENT_BOX (self), FALSE);
	
	g_signal_connect (G_OBJECT (self), "button-press-event",
		G_CALLBACK (moko_finger_scroll_button_press_cb), NULL);
	g_signal_connect (G_OBJECT (self), "button-release-event",
		G_CALLBACK (moko_finger_scroll_button_release_cb), NULL);
	g_signal_connect (G_OBJECT (self), "motion-notify-event",
		G_CALLBACK (moko_finger_scroll_motion_notify_cb), NULL);
}

GtkWidget *
moko_finger_scroll_new (void)
{
	return g_object_new (MOKO_TYPE_FINGER_SCROLL, NULL);
}
