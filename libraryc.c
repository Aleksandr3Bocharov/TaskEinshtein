// Copyright 2023 Aleksandr Bocharov
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
// 2023-07-18
// https://github.com/Aleksandr3Bocharov/TaskEinshtein

/* 

Модуль "libraryc" с реализацией на языке C функций для программ на языке Рефал-05.
https://github.com/Mazdaywik/Refal-05

Модуль "libraryc" использует библиотеку GTK3.
https://docs.gtk.org/gtk3/

*/

#include <gtk\gtk.h>
#include <stdio.h>
#include <string.h>
#include "refal05rts.h"

/* 

<PutChar s.Char> ==

s.Char ::= s.CHAR

*/
R05_DEFINE_ENTRY_FUNCTION(PutChar, "PutChar") {
  struct r05_node *callee = arg_begin->next;
  struct r05_node *p;

#define CHECK_PUTCHAR(putchar_call) \
  ((putchar_call) != EOF ? (void) 0 \
  : r05_builtin_error_errno("Error in call " #putchar_call))
  
  p = callee->next;

  if (p != arg_end)
    switch (p->tag)
    {
      case R05_DATATAG_CHAR:
        CHECK_PUTCHAR(putchar(p->info.char_));
        break;

      default:
        r05_recognition_impossible();
    }
  else
    r05_recognition_impossible();

#undef CHECK_PUTCHAR

  if (p->next != arg_end)
    r05_recognition_impossible();

  r05_splice_to_freelist(arg_begin, arg_end);

}

/*

<GetChar> == s.Char

s.Char ::= s.CHAR

*/
R05_DEFINE_ENTRY_FUNCTION(GetChar, "GetChar") {
  struct r05_node *callee = arg_begin->next;

  if (callee->next != arg_end)
    r05_recognition_impossible();

  r05_reset_allocator();
  
  int cur_char = getchar();

  if (cur_char == EOF)
    r05_alloc_number(0);
  else
    r05_alloc_char((char) cur_char);

  r05_splice_from_freelist(arg_begin);
  r05_splice_to_freelist(arg_begin, arg_end);
}

/*

<GTKInit> ==

*/
R05_DEFINE_ENTRY_FUNCTION(GTKInit, "GTKInit") {
  struct r05_node *callee = arg_begin->next;

  if (callee->next != arg_end)
    r05_recognition_impossible();

  r05_reset_allocator();

  gtk_init(0, NULL);

  r05_splice_from_freelist(arg_begin);
  r05_splice_to_freelist(arg_begin, arg_end);
}

/*

<OpenFileDialog> == e.bfFileName

e.bfFileName ::= s.CHAR*

*/
R05_DEFINE_ENTRY_FUNCTION(OpenFileDialog, "OpenFileDialog") {
  struct r05_node *callee = arg_begin->next;

  if (callee->next != arg_end)
    r05_recognition_impossible();

  r05_reset_allocator();

  GtkWidget *dialog;

  dialog = gtk_file_chooser_dialog_new("Открыть файл",
                                      NULL,
                                      GTK_FILE_CHOOSER_ACTION_OPEN,
                                      GTK_STOCK_OPEN,
                                      GTK_RESPONSE_ACCEPT,
                                      GTK_STOCK_CANCEL,
                                      GTK_RESPONSE_CANCEL,
                                      NULL);

  gint res = gtk_dialog_run(GTK_DIALOG(dialog));
  if (res == GTK_RESPONSE_ACCEPT)
  {
    char *filename;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    filename = gtk_file_chooser_get_filename(chooser); 

    r05_alloc_string(filename);

    g_free (filename);
  }  

  gtk_widget_destroy(dialog);

  r05_splice_from_freelist(arg_begin);
  r05_splice_to_freelist(arg_begin, arg_end);
}

/*

<SaveFileDialog e.iFileName> == e.FileName

e.iFileName ::= s.CHAR+
e.FileName ::= s.CHAR*

*/
R05_DEFINE_ENTRY_FUNCTION(SaveFileDialog, "SaveFileDialog") {
  struct r05_node *fname_b, *fname_e;
  char ifilename[FILENAME_MAX + 1];
  size_t ifilename_len;

  /* сопоставление с образцом */
  r05_prepare_argument(&fname_b, &fname_e, arg_begin, arg_end);
  ifilename_len = r05_read_chars(ifilename, FILENAME_MAX, &fname_b, &fname_e);
  ifilename[ifilename_len] = '\0';

  if (ifilename_len == 0)
    r05_recognition_impossible();

  if (! r05_empty_seq(fname_b, fname_e))
  {
    struct r05_node *p = fname_b;
    
    while (p != fname_e->next && p->tag == R05_DATATAG_CHAR)
      p = p->next;

    if (p == fname_e->next)
      r05_builtin_error("very long filename");
    else
      r05_recognition_impossible();
  }

  r05_reset_allocator();

  GtkWidget *dialog;
  GtkFileChooser *chooser;

  dialog = gtk_file_chooser_dialog_new("Сохранить файл",
                                      NULL,
                                      GTK_FILE_CHOOSER_ACTION_SAVE,
                                      GTK_STOCK_SAVE,
                                      GTK_RESPONSE_ACCEPT,
                                      GTK_STOCK_CANCEL,
                                      GTK_RESPONSE_CANCEL,
                                      NULL);
  chooser = GTK_FILE_CHOOSER(dialog);

  gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

  gtk_file_chooser_set_current_name(chooser, (ifilename));
  
  gint res = gtk_dialog_run(GTK_DIALOG(dialog));
  if (res == GTK_RESPONSE_ACCEPT)
  {
    char *filename;
    filename = gtk_file_chooser_get_filename(chooser);
    
    r05_alloc_string(filename);

    g_free(filename);
  }

  gtk_widget_destroy(dialog);

  r05_splice_from_freelist(arg_begin);
  r05_splice_to_freelist(arg_begin, arg_end);
}

/*

<MessageBox s.Type e.Message> ==

s.Type ::= Info | Warning | Error
e.Message ::= s.CHAR+

*/
R05_DEFINE_ENTRY_FUNCTION(MessageBox, "MessageBox") {
  struct r05_node *callee = arg_begin->next;
  struct r05_node *ident = callee->next;
  
  GtkMessageType type;

  if (ident == arg_end)
    r05_recognition_impossible(); 

  if (R05_DATATAG_FUNCTION != ident->tag)
    r05_recognition_impossible();

  if (!strcmp(ident->info.function->name, "Info"))
    type = GTK_MESSAGE_INFO;
  else if (!strcmp(ident->info.function->name, "Warning"))
    type = GTK_MESSAGE_WARNING;
  else if (!strcmp(ident->info.function->name, "Error"))
    type = GTK_MESSAGE_ERROR;  
  else
    r05_recognition_impossible();  
    
  struct r05_node *msg_b, *msg_e;

#define MESSAGE_MAX 2048
  char message[MESSAGE_MAX + 1];
  size_t message_len;

  r05_prepare_argument(&msg_b, &msg_e, callee, arg_end);
  message_len = r05_read_chars(message, MESSAGE_MAX, &msg_b, &msg_e);
  message[message_len] = '\0';

  if (message_len == 0)
    r05_recognition_impossible();

  if (!r05_empty_seq(msg_b, msg_e))
  {
    struct r05_node *p = msg_b;
    
    while (p != msg_e->next && p->tag == R05_DATATAG_CHAR)
      p = p->next;

    if (p == msg_e->next)
      r05_builtin_error("very long message");
    else
      r05_recognition_impossible();
  }

#undef MESSAGE_MAX

  r05_reset_allocator();
  
  GtkWidget *dialog;
  GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
  dialog = gtk_message_dialog_new(NULL,
                                  flags,
                                  type,
                                  GTK_BUTTONS_OK,
                                  "%s",
                                  message);
  
  gtk_dialog_run(GTK_DIALOG(dialog));
  
  gtk_widget_destroy(dialog);

  r05_splice_from_freelist(arg_begin);
  r05_splice_to_freelist(arg_begin, arg_end);
}