/* vi: set sw=2: */
/*
 * Copyright (C) 2007 by OpenMoko, Inc.
 * Written by OpenedHand Ltd <info@openedhand.com>
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef __MOKO_JOURNAL_H__
#define  __MOKO_JOURNAL_H__

#include <glib.h>
#include "moko-time.h" 

/************************************************************
 * this API abstracts the process of adding
 * journal entries into the default system
 * journal. Journal entries contain information
 * about communation history like calls, sms, or,
 * emails issued or received.
 ***********************************************************/

typedef struct _MokoJournal MokoJournal ;
typedef struct _MokoJEntry MokoJEntry ;
typedef struct _MokoJEmailInfo MokoJEmailInfo ;
typedef struct _MokoJSMSInfo MokoJSMSInfo ;
typedef struct _MokoJMMSInfo MokoJMMSInfo ;
typedef struct _MokoJCallInfo MokoJCallInfo ;

/**
 * this represents the primary type of
 * a journal entry.
 */
typedef enum _MokoJEntryType {
  UNDEF_ENTRY=0,
  EMAIL_JOURNAL_ENTRY,
  SMS_JOURNAL_ENTRY,
  MMS_JOURNAL_ENTRY,
  CALL_JOURNAL_ENTRY,
  NB_OF_ENTRY_TYPES /*must always be the last*/
} MokoJEntryType ;

/*<journal management>*/
/**
 * moko_journal_open_default:
 *
 * Opens the default journal.
 *
 * Return value: a pointer to the journal object
 */
MokoJournal* moko_journal_open_default () ;

/**
 * moko_journal_close:
 * @journal: the journal to close
 *
 * Close the journal previously opened with moko_journal_open_default().
 * This function deallocates the memory of the Journal object.
 */
void moko_journal_close (MokoJournal *journal) ;

/**
 * moko_journal_add_entry:
 * @journal: the current instance of journal
 * @entry: the new entry to add to the journal. Must be non NULL.
 * The journal is responsible
 * of deallocating the memory of the entry object.
 *
 * Add a journal entry to the journal
 *
 * Return value: TRUE if the entry got successfully added to the journal,
 * FALSE otherwise
 */
gboolean moko_journal_add_entry (MokoJournal *journal, MokoJEntry *entry) ;

/**
 * moko_journal_get_nb_entries:
 * @journal: the current instance of journal
 *
 * Return value: the number of entries in the journal or a negative value
 * in case of error.
 */
int moko_journal_get_nb_entries (MokoJournal *journal) ;

/**
 * moko_journal_get_entry_at:
 * @journal: the current instance of journal
 * @index: the index to get the journal entry from
 * @entry: out parameter. the resulting journal entry
 *
 * Get the journal entry at a given index.
 *
 * Return value: TRUE in case of success, FALSE otherwise.
 */
gboolean moko_journal_get_entry_at (MokoJournal *journal,
                                    guint index,
                                    MokoJEntry **entry) ;

/**
 * moko_journal_remove_entry_at:
 * @journal: the current instance of journal
 * @index: the index to remove the entry from
 *
 * Remove a journal entry from index #index
 */
gboolean moko_journal_remove_entry_at (MokoJournal *journal,
                                       guint index) ;

/**
 * moko_journal_weite_to_storage:
 * @journal: the journal to save to storage
 *
 * Saves the journal to persistent storage (e.g disk) using the
 * appropriate backend. The backend currently used is evolution data server
 *
 * Return value: TRUE in case of success, FALSE otherwise
 */
gboolean moko_journal_write_to_storage (MokoJournal *journal) ;

/**
 * moko_journal_load_from_storage:
 * @journal: the journal to load entries into
 *
 * Read the journal entries stored in the persistent storage (filesystem)
 * and load then into the current instance of MokoJournal.
 *
 * Return value: TRUE in case of success, FALSE otherwise
 */
gboolean moko_journal_load_from_storage (MokoJournal *journal) ;

/*<journal entries querying>*/
/*</journal entries querying>*/

/*</journal management>*/


/*<journal entries management>*/

/**
 * moko_j_entry_new:
 * @type: the type of journal entry
 *
 * Create a Journal entry with no properties set.
 * Use the JEntry accessors to get/set properties.
 *
 * Return value: the newly created journal entry object
 */
MokoJEntry* moko_j_entry_new (MokoJEntryType type) ;

/**
 * moko_j_entry_free:
 * @entry: the entry to free
 *
 * Deallocate the memory of the journal entry object
 */
void moko_j_entry_free (MokoJEntry *entry) ;

/**
 * moko_j_entry_get_type:
 * @entry: the current journal entry
 *
 * get the primary type of the journal entry
 *
 * Return value: the type of the journal entry
 */
MokoJEntryType moko_j_entry_get_type (MokoJEntry *entry) ;

/**
 * moko_j_entry_set_type:
 * @entry: the current instance of journal entry
 * @type: the new type
 *
 * Set the type of the journal entry
 */
void moko_j_entry_set_type (MokoJEntry *entry, MokoJEntryType type) ;

/**
 * moko_j_entry_get_contact_uid:
 * @entry: the current instance of journal entry
 *
 * get the contact uid
 *
 * Return value: the UID of the contact. It can be NULL. Client code
 * must not deallocate or attempt to alter it.
 */
const gchar* moko_j_entry_get_contact_uid (MokoJEntry *entry) ;

/**
 * moko_j_entry_set_contact_uid:
 * @entry: the current instance of journal entry
 * @uid: the uid to set. This string is copied so the client code
 * must free it.
 *
 * Associate a new contact UID to the journal entry uid.
 * The UID is copied by this function so the caller is reponsible of
 * taking care of the uid string lifecycle.
 */
void  moko_j_entry_set_contact_uid (MokoJEntry *entry, const gchar *uid) ;

/**
 * moko_j_entry_get_summary:
 * @entry: the current instance of journal entry
 *
 * get the summary of the journal entry
 *
 * Return value: the summary of the journal entry. It can be NULL.
 * Client code must not deallocate or alter it.
 */
const gchar* moko_j_entry_get_summary (MokoJEntry *entry) ;

/**
 * moko_j_entry_set_summary:
 * @entry: the current instance of journal entry
 * @summary: the new summary of the journal entry. It is copied
 * so client code is reponsible of its lifecyle.
 *
 * Set the summary of the journal entry
 */
void moko_j_entry_set_summary (MokoJEntry *entry, const gchar* summary) ;

/**
 * moko_j_entry_get_dtdstart:
 * @entry: the current instance of journal entry
 *
 * get the starting date associated to the journal entry
 *
 * Return value: an icaltimetype representing the starting date expected.
 * It can be NULL. Client code must not deallocate it.
 */
const MokoTime* moko_j_entry_get_dtstart (MokoJEntry *entry) ;

/**
 * moko_j_entry_set_dtstart:
 * @entry: the current instance of journal entry
 * @dtstart: the new starting date associated to the journal entry.
 */
void moko_j_entry_set_dtstart (MokoJEntry *entry, MokoTime* dtstart);

/*<email info>*/

/**
 * moko_j_entry_get_email_info:
 * @entry: the current instance of journal entry
 * @info: extra information attached to the email info, or NULL.
 * Client code must *NOT* of deallocate the returned info.
 * It is the duty of the MokoJEntry code to deallocate it when
 * necessary
 *
 * Return value: TRUE if the call succeeded, FALSE otherwise.
 */
gboolean moko_j_entry_get_email_info (MokoJEntry *entry,
                                      MokoJEmailInfo **info) ;

/**
 * moko_j_email_info_get_was_sent:
 * @info: the current instance of email info
 *
 * Get a boolean property stating if the email was sent or received.
 *
 * Return value: TRUE if the email was sent, false if it was received
 */
gboolean moko_j_email_info_get_was_sent (MokoJEmailInfo *info) ;

/**
 * moko_j_email_info_set_was_sent:
 * @info: the current instance of email info
 * @was_sent: TRUE if the email was sent, FALSE if it was received
 *
 * Set a boolean property stating if the email was sent or received
 */
void moko_j_email_info_set_was_sent (MokoJEmailInfo *info, gboolean was_sent) ;

/*</email info>*/


/*</journal entries management>*/

#endif /*__MOKO_JOURNAL_H__*/
