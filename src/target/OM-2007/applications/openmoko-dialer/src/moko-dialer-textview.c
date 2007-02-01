/*  moko-dialer-textview.c
 *
 *  Authored by Tony Guan<tonyguan@fic-sh.com.cn>
 *
 *  Copyright (C) 2006 FIC Shanghai Lab
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Public License as published by
 *  the Free Software Foundation; version 2 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  Current Version: $Rev$ ($Date) [$Author: Tony Guan $]
 
 */

#include "moko-dialer-textview.h"
#include "moko-dialer-declares.h"
#include "error.h"
G_DEFINE_TYPE (MokoDialerTextview, moko_dialer_textview, GTK_TYPE_TEXT_VIEW)
     enum
     {
       CLICKED_SIGNAL,
       HOLD_SIGNAL,
       LAST_SIGNAL
     };

//forward definition
//static void  moko_dialer_textview_pressed  (MokoDigitButton * button, GdkEventButton  *event,gpointer data);
//static void moko_dialer_textview_pressed (MokoDigitButton * button, gpointer data);

     static gint moko_dialer_textview_signals[LAST_SIGNAL] = { 0 };

static void
moko_dialer_textview_class_init (MokoDialerTextviewClass * class)
{

  GtkObjectClass *object_class;

  object_class = (GtkObjectClass *) class;


}


static void
moko_dialer_textview_init (MokoDialerTextview * moko_dialer_textview)
{

  GtkTextView *textview = 0;
  GtkTextBuffer *buffer;
  GdkColor color;

  textview = &moko_dialer_textview->textview;
  buffer = gtk_text_view_get_buffer (textview);
  moko_dialer_textview->font_desc_textview = NULL;
  moko_dialer_textview->tag_for_inputed = NULL;
  moko_dialer_textview->tag_for_cursor = NULL;
  moko_dialer_textview->tag_for_autofilled = NULL;

  gdk_color_parse ("black", &color);
//                      gtk_widget_modify_bg(textview,GTK_STATE_NORMAL,&color);
  gtk_widget_modify_base (textview, GTK_STATE_NORMAL, &color);




  gtk_widget_set_size_request (GTK_WIDGET (textview), 480, 92);
  GTK_WIDGET_UNSET_FLAGS (textview, GTK_CAN_FOCUS);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), FALSE);
  gtk_text_view_set_accepts_tab (GTK_TEXT_VIEW (textview), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview), GTK_WRAP_CHAR);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (textview), FALSE);
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (textview), 1);
  gtk_text_view_set_right_margin (GTK_TEXT_VIEW (textview), 1);



  PangoFontDescription *font_desc_textview = NULL;
  font_desc_textview = pango_font_description_new ();

  // set the default font for the textview.
  pango_font_description_set_size (font_desc_textview, 32 * PANGO_SCALE);

  if (font_desc_textview)
  {
    gtk_widget_modify_font (GTK_WIDGET (moko_dialer_textview),
                            font_desc_textview);
    // save it to the structure for later usage.
    moko_dialer_textview->font_desc_textview = font_desc_textview;
  }
  // create the formatting tag;
  moko_dialer_textview->tag_for_inputed =
    gtk_text_buffer_create_tag (buffer, "tag_input", "foreground", "#FF8000",
                                NULL);
  moko_dialer_textview->tag_for_cursor =
    gtk_text_buffer_create_tag (buffer, "tag_cursor", "weight",
                                "PANGO_WEIGHT_BOLD", NULL);
  moko_dialer_textview->tag_for_autofilled =
    gtk_text_buffer_create_tag (buffer, "tag_filled", "foreground", "#FFFF00",
                                NULL);
  moko_dialer_textview->sensed = FALSE;

}


GtkWidget *
moko_dialer_textview_new ()
{
  MokoDialerTextview *dp;

  dp = (MokoDialerTextview *) g_object_new (MOKO_TYPE_DIALER_TEXTVIEW, NULL);
  return GTK_WIDGET (dp);

}

/**
 * @brief moko_dialer_textview_set_color(MokoDialerTextview *moko_dialer_textview)
 *
 * set the text left to the cursor to black, and right to red.
 *
 * @param moko_dialer_textview the display area
 * @param len the char length,if it exceeds 13,then automatically decrease the size.
 * @return  void
 * @retval void
 */

void
moko_dialer_textview_set_color (MokoDialerTextview * moko_dialer_textview)
{

  GtkTextBuffer *buffer;
  gint len;
  GtkTextIter start, cursoriter_1, cursoriter;
  GtkTextIter end;

//text_view= lookup_widget (GTK_WIDGET(button), "textview");
  /* Obtaining the buffer associated with the widget. */
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (moko_dialer_textview));

  gtk_text_buffer_get_iter_at_mark (buffer,
                                    &cursoriter,
                                    gtk_text_buffer_get_insert (buffer));

  gtk_text_buffer_get_start_iter (buffer, &start);
  gtk_text_buffer_get_end_iter (buffer, &end);

  gint cur = gtk_text_iter_get_offset (&cursoriter);

  if (cur > 0)
  {
    gtk_text_buffer_remove_all_tags (buffer, &start, &end);

    gtk_text_buffer_get_iter_at_offset (buffer, &cursoriter_1, cur - 1);
    gtk_text_buffer_apply_tag (buffer, moko_dialer_textview->tag_for_inputed,
                               &start, &cursoriter);
    gtk_text_buffer_apply_tag (buffer,
                               moko_dialer_textview->tag_for_autofilled,
                               &cursoriter, &end);
    gtk_text_buffer_apply_tag (buffer, moko_dialer_textview->tag_for_cursor,
                               &cursoriter_1, &cursoriter);

  }
  else
  {                             // cur==0
    gtk_text_buffer_apply_tag (buffer,
                               moko_dialer_textview->tag_for_autofilled,
                               &cursoriter, &end);
  }


  len = gtk_text_buffer_get_char_count (buffer);
  if (len >= 12 && len <= 64)
  {
    if (moko_dialer_textview->font_desc_textview)
      pango_font_description_set_size (moko_dialer_textview->
                                       font_desc_textview, 32 * PANGO_SCALE);
  }
  else if (len >= 9 && len < 12)
  {
    if (moko_dialer_textview->font_desc_textview)
      pango_font_description_set_size (moko_dialer_textview->
                                       font_desc_textview, 48 * PANGO_SCALE);
  }
  else if (len >= 0 && len < 9)
  {
    if (moko_dialer_textview->font_desc_textview)
      pango_font_description_set_size (moko_dialer_textview->
                                       font_desc_textview, 64 * PANGO_SCALE);
  }

  gtk_widget_modify_font (GTK_WIDGET (moko_dialer_textview),
                          moko_dialer_textview->font_desc_textview);

  gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (moko_dialer_textview),
                                      gtk_text_buffer_get_insert (buffer));

}



 /**
 * @brief moko_dialer_textview_insert  
 *
 * This function should be called upon the keypad is clicked in dialer window.
 *
 * @param button the button hit.
 * @param number the number to be added to the display.
 * @return  int
 * @retval 
 */

gint
moko_dialer_textview_insert (MokoDialerTextview * moko_dialer_textview,
                             const gchar * number)
{

  gint len = 0;

  GtkTextBuffer *buffer;
  GtkTextIter start;
  GtkTextIter end;
  GtkTextIter selectioniter, insertiter;
  GtkTextMark *selectmark, *insertmark;

//DBG_MESSAGE("number=%s",number);

  /* Obtaining the buffer associated with the widget. */
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (moko_dialer_textview));

  selectmark = gtk_text_buffer_get_selection_bound (buffer);
  insertmark = gtk_text_buffer_get_insert (buffer);
//get current cursor iterator
  gtk_text_buffer_get_iter_at_mark (buffer, &insertiter, insertmark);
  gtk_text_buffer_get_iter_at_mark (buffer, &selectioniter, selectmark);
  // to see whether there is a selection range.
  if (gtk_text_iter_get_offset (&insertiter) !=
      gtk_text_iter_get_offset (&selectioniter))
  {
    // yes, first delete the range.
    gtk_text_buffer_delete (buffer, &selectioniter, &insertiter);
    insertmark = gtk_text_buffer_get_insert (buffer);
    gtk_text_buffer_get_iter_at_mark (buffer, &insertiter, insertmark);
  }

  gtk_text_buffer_get_start_iter (buffer, &start);
  gtk_text_buffer_get_end_iter (buffer, &end);


  len = gtk_text_buffer_get_char_count (buffer);
  // len=strlen(codestring);
  if (len >= 0 && len < MOKO_DIALER_MAX_NUMBER_LEN)
  {

    gtk_text_buffer_insert_at_cursor (buffer, number, strlen (number));
    len = len + strlen (number);
  }
  else
  {
//      DBG_WARN("INPUT EXCEEDS %d,reset to null!",MAXDIALNUMBERLEN);
    gtk_text_buffer_set_text (buffer, number, -1);
    len = 1;
  }
  // reget the cursor iter.
  insertmark = gtk_text_buffer_get_insert (buffer);
//get current cursor iterator
  gtk_text_buffer_get_iter_at_mark (buffer, &insertiter, insertmark);
  // get the inputed string lengh.
  len = gtk_text_iter_get_offset (&insertiter);
//      DBG_MESSAGE("the current cursor offset is %d",len);

/*
	if(len>=MINSENSATIVELEN)
	{//here we start to search the contacts.
		rebuild_contact_view(text_view,1);		
	}
*/

  moko_dialer_textview_set_color (moko_dialer_textview);
  return len;
}


//get the input section of the textview 
//if ALL=true, get whole text
//else only get the inputed digits.
gint
moko_dialer_textview_get_input (MokoDialerTextview * moko_dialer_textview,
                                gchar * input, int ALL)
{
  gchar *codestring;
  GtkTextBuffer *buffer;
  GtkTextIter start;
  GtkTextIter end;
  GtkTextIter insertiter;
  GtkTextMark *insertmark;

//DBG_ENTER();

/* Obtaining the buffer associated with the widget. */
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (moko_dialer_textview));

//get current cursor iterator
  insertmark = gtk_text_buffer_get_insert (buffer);
  gtk_text_buffer_get_iter_at_mark (buffer, &insertiter, insertmark);
//get start & end iterator
  gtk_text_buffer_get_start_iter (buffer, &start);
  gtk_text_buffer_get_end_iter (buffer, &end);

  if (gtk_text_iter_get_offset (&insertiter) ==
      gtk_text_iter_get_offset (&start))
  {
    strcpy (input, "");
    return 0;
  }
// DBG_TRACE();
  if (ALL)
    /* Get the entire buffer text. */
    codestring = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
  else
    codestring =
      gtk_text_buffer_get_text (buffer, &start, &insertiter, FALSE);
//DBG_MESSAGE("%s",codestring);
  strcpy (input, codestring);
// DBG_TRACE();
  g_free (codestring);
  return 1;
}

//delete all the input 
int
moko_dialer_textview_empty (MokoDialerTextview * moko_dialer_textview)
{
  GtkTextBuffer * buffer;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (moko_dialer_textview));

  gtk_text_buffer_set_text (buffer, "", -1);
  return 1;
}

///delete the selection or one character.
int
moko_dialer_textview_delete (MokoDialerTextview * moko_dialer_textview)
{
  GtkTextBuffer *buffer;
  GtkTextIter selectioniter, insertiter;
  GtkTextMark *selectmark, *insertmark;
  gint len;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (moko_dialer_textview));

  selectmark = gtk_text_buffer_get_selection_bound (buffer);
  insertmark = gtk_text_buffer_get_insert (buffer);
//get current cursor iterator
  gtk_text_buffer_get_iter_at_mark (buffer, &insertiter, insertmark);
  gtk_text_buffer_get_iter_at_mark (buffer, &selectioniter, selectmark);
  // to see whether there is a selection range.
  if (gtk_text_iter_get_offset (&insertiter) !=
      gtk_text_iter_get_offset (&selectioniter))
  {
    // yes, first delete the range.
    gtk_text_buffer_delete (buffer, &selectioniter, &insertiter);
  }
  else
  {
    // no selection, then just perform backspace.
    gtk_text_buffer_backspace (buffer, &insertiter, TRUE, TRUE);
  }

//now we get the inputed string length. 
  insertmark = gtk_text_buffer_get_insert (buffer);
  // get current cursor iterator
  gtk_text_buffer_get_iter_at_mark (buffer, &insertiter, insertmark);
  len = gtk_text_iter_get_offset (&insertiter);

  return 1;

}

//autofill the string to the inputed digits string on the textview

int
moko_dialer_textview_fill_it (MokoDialerTextview * moko_dialer_textview,
                              gchar * string)
{


  GtkTextBuffer *buffer;
  GtkTextIter start;
  GtkTextIter end;
  GtkTextIter insertiter;
  GtkTextMark *insertmark;
  gint offset;
  gint offsetend;
  gint offsetstart;

//DBG_ENTER();
//DBG_MESSAGE("Sensative string:%s",string);

/* Obtaining the buffer associated with the widget. */
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (moko_dialer_textview));

//get current cursor iterator
  insertmark = gtk_text_buffer_get_insert (buffer);
  gtk_text_buffer_get_iter_at_mark (buffer, &insertiter, insertmark);
//get start & end iterator
  gtk_text_buffer_get_start_iter (buffer, &start);
  gtk_text_buffer_get_end_iter (buffer, &end);
  offsetend = gtk_text_iter_get_offset (&end);
  offset = gtk_text_iter_get_offset (&insertiter);
//if startpos=endpos, that means we didn't input anything
//so we just insert the text.

  offsetstart = gtk_text_iter_get_offset (&start);
  if (offsetend == offsetstart)
  {

    gtk_text_buffer_set_text (buffer, string, -1);

    gtk_text_buffer_get_start_iter (buffer, &start);
    gtk_text_buffer_place_cursor (buffer, &start);
    moko_dialer_textview_set_color (moko_dialer_textview);
    moko_dialer_textview->sensed = TRUE;
    // gtk_widget_grab_focus(text_view);
    return 1;

  }

/* Get the entire buffer text. */

//codestring = gtk_text_buffer_get_text (buffer, &start, &insertiter, FALSE);

  gtk_text_buffer_delete (buffer, &insertiter, &end);


//reget current cursor iterator
  insertmark = gtk_text_buffer_get_insert (buffer);
  gtk_text_buffer_get_iter_at_mark (buffer, &insertiter, insertmark);

//here we have to call the get sensentivestring to get "139" or something.
//gtk_text_buffer_insert_with_tags_by_name(buffer,&insertiter,"139",3,tag_name);
  if (string != 0)
  {
    if (strlen (string) > 0)
    {
      gtk_text_buffer_insert (buffer, &insertiter, string, strlen (string));

//reget current cursor iterator
      insertmark = gtk_text_buffer_get_insert (buffer);
      gtk_text_buffer_get_iter_at_mark (buffer, &insertiter, insertmark);
      gtk_text_iter_set_offset (&insertiter, offset);
      // set the private data of sensed.
    }
  }

//setback the cursor position
  gtk_text_buffer_place_cursor (buffer, &insertiter);

  moko_dialer_textview_set_color (moko_dialer_textview);

//gtk_widget_grab_focus(text_view);
//g_free (codestring );

//DBG_LEAVE();
  return 1;
}

gint
moko_dialer_textview_confirm_it (MokoDialerTextview * moko_dialer_textview,
                                 const gchar * string)
{

  GtkTextBuffer *buffer;
  GtkTextIter end;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (moko_dialer_textview));


  gtk_text_buffer_set_text (buffer, string, -1);


  gtk_text_buffer_get_end_iter (buffer, &end);
//set the cursor to the end of the buffer
  gtk_text_buffer_place_cursor (buffer, &end);

  moko_dialer_textview_set_color (moko_dialer_textview);
//

  return 1;

}
