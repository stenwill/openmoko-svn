/* detail-area.c
 *
 * Authored By Alex Tang <alex@fic-sh.com.cn>
 *
 * Copyright (C) 2006-2007 OpenMoko Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Public License as published by
 * the Free Software Foundation; version 2.1 of the license.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Public License for more details.
 *
 * Current Version: $Rev$ ($Date: 2006/10/05 17:38:14 $) [$Author: Alex $]
 *
 */

#include "detail-area.h"

GtkWidget* detail_area_mode_read (DetailArea* self);

G_DEFINE_TYPE (DetailArea, detail_area, MOKO_TYPE_SCROLLED_PANE)

#define DETAIL_AREA_GET_PRIVATE(o)     (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_DETAIL_AREA, DetailAreaPrivate))

typedef struct _DetailAreaPrivate{
  GtkWidget* from_label;
  GtkWidget* date_label;
  GtkWidget* details;
}DetailAreaPrivate;

/* parent class pointer */
static GtkWindowClass* parent_class = NULL;

static void
detail_area_dispose (GObject *object)
{
  if (G_OBJECT_CLASS (detail_area_parent_class)->dispose)
    G_OBJECT_CLASS (detail_area_parent_class)->dispose (object);
}

static void
detail_area_finalize (GObject *object)
{
  G_OBJECT_CLASS (detail_area_parent_class)->finalize (object);
}

static void
detail_area_class_init (DetailAreaClass *klass)
{
  /* hook parent */
  GObjectClass* object_class = G_OBJECT_CLASS (klass);
  parent_class = g_type_class_peek_parent(klass);

  g_type_class_add_private (klass, sizeof(DetailAreaPrivate));

  object_class->dispose = detail_area_dispose;
  object_class->finalize = detail_area_finalize;

}

static void
detail_area_init (DetailArea *self)
{
  self->notebook = GTK_NOTEBOOK( gtk_notebook_new() );
  gtk_notebook_append_page (self->notebook,detail_area_mode_read(self),NULL);
  gtk_notebook_set_show_tabs (self->notebook,FALSE);
  gtk_notebook_set_show_border (self->notebook,FALSE);

  moko_scrolled_pane_pack (MOKO_SCROLLED_PANE (self), self->notebook);
}

GtkWidget* detail_area_new (void)
{
  return GTK_WIDGET(g_object_new(TYPE_DETAIL_AREA, NULL ));
}

GtkWidget* detail_area_get_notebook(DetailArea* self)
{
  return GTK_WIDGET(self->notebook);
}

GtkWidget* detail_area_mode_read (DetailArea* self)
{
  DetailAreaPrivate* priv = DETAIL_AREA_GET_PRIVATE(self);

  /* create detail box */
  self->detailbox = GTK_VBOX(gtk_vbox_new(FALSE,0));

  GtkWidget* headerbox = gtk_vbox_new(FALSE,0);
  GtkWidget* hbox = gtk_hbox_new(FALSE,0);
  priv->from_label = gtk_label_new ("Alex");
  gtk_misc_set_alignment (GTK_MISC (priv->from_label),1,0.5);
  priv->date_label = gtk_label_new ("Hello");
  gtk_misc_set_alignment (GTK_MISC (priv->date_label),1,0.5);

  GtkWidget* cellalign = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_alignment_set_padding (GTK_ALIGNMENT(cellalign), 5,5,5,5);
  GtkWidget* label = gtk_label_new ("From:");
  gtk_misc_set_alignment (GTK_MISC (label),1,0.5);
  gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox),priv->from_label,FALSE,FALSE,0);
  gtk_container_add(GTK_CONTAINER(cellalign),hbox);
  gtk_box_pack_start(GTK_BOX(headerbox),cellalign,FALSE,FALSE,0);

  cellalign = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_alignment_set_padding (GTK_ALIGNMENT(cellalign), 5,5,5,5);
  hbox = gtk_hbox_new(FALSE,0);
  label = gtk_label_new ("Date:");
  gtk_misc_set_alignment (GTK_MISC (label),1,0.5);
  gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox),priv->date_label,FALSE,FALSE,0);
  gtk_container_add(GTK_CONTAINER(cellalign),hbox);
  gtk_box_pack_start(GTK_BOX(headerbox),cellalign,FALSE,FALSE,0);

  GtkWidget* hseparator = gtk_hseparator_new();
  GtkWidget* detailAlign = gtk_alignment_new(0, 0, 0, 0);
  gtk_alignment_set_padding (GTK_ALIGNMENT(detailAlign), 10, 10, 10, 50);
  /*GtkWidget* details = gtk_label_new("this is the detail");*/
  priv->details = gtk_label_new( "Add your widget for showing details for the selected\n"
                            "\ndata entry here\n \n \n \n \n \n \n \nThis particular label\n \nis very long\n"
                            "\nto make the fullscreen\n \ntrigger more interesting\n \n \n");
  gtk_widget_set_size_request (priv->details,420,-1);
  gtk_label_set_line_wrap (GTK_LABEL(priv->details),TRUE);
  gtk_misc_set_alignment (GTK_MISC (priv->details),0.1,0.5);
  gtk_container_add (GTK_CONTAINER(detailAlign),priv->details);
  gtk_box_pack_start(GTK_BOX(self->detailbox),headerbox,FALSE,TRUE,0);
  gtk_box_pack_start(GTK_BOX(self->detailbox),hseparator,FALSE,TRUE,0);
  gtk_box_pack_start(GTK_BOX(self->detailbox),detailAlign,FALSE,TRUE,0);

  return GTK_WIDGET(self->detailbox);
}

void detail_read_message (DetailArea* self, message* msg)
{
  DetailAreaPrivate* priv = DETAIL_AREA_GET_PRIVATE(self);

  if(msg != NULL)
    {
      gtk_label_set_text(GTK_LABEL(priv->from_label), msg->name);
      gtk_label_set_text(GTK_LABEL(priv->date_label), msg->subject);
      gtk_label_set_text(GTK_LABEL(priv->details), msg->folder);
      g_free(msg);
    }
  else
    {
      gtk_label_set_text(GTK_LABEL(priv->from_label), "");
      gtk_label_set_text(GTK_LABEL(priv->date_label), "");
      gtk_label_set_text(GTK_LABEL(priv->details), "please select a message");
    }

  gtk_notebook_set_current_page (self->notebook,PAGE_MODE_READ);
}
