// Copyright 2023 Aleksandr Bocharov
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
// 2023-07-18
// https://github.com/Aleksandr3Bocharov/TaskEinshtein

/* 

Модуль "taskeinshteinc" с реализацией на языке C функций для программы "TaskEinshtein".
https://github.com/Aleksandr3Bocharov/TaskEinshtein

Модуль "taskeinshteinc" использует библиотеку GTK3.
https://docs.gtk.org/gtk3/

*/

#include <gtk\gtk.h>
#include <stdio.h>
#include "refal05rts.h"

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

<MessageBox e.Message> ==

e.Message ::= s.CHAR+

*/
R05_DEFINE_ENTRY_FUNCTION(MessageBox, "MessageBox") {
  struct r05_node *msg_b, *msg_e;

#define MESSAGE_MAX 2048
  char message[MESSAGE_MAX + 1];
  size_t message_len;

  r05_prepare_argument(&msg_b, &msg_e, arg_begin, arg_end);
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
  dialog = gtk_message_dialog_new(NULL,
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_INFO,
                                  GTK_BUTTONS_OK,
                                  "%s",
                                  message);
  
  gtk_dialog_run(GTK_DIALOG(dialog));
  
  gtk_widget_destroy(dialog);

  r05_splice_from_freelist(arg_begin);
  r05_splice_to_freelist(arg_begin, arg_end);
}


/*

<TreeView> ==

*/
enum {
    COLUMN_TITLE,
    COLUMN_ARTIST,
    COLUMN_CATALOGUE,
    N_COLUMNS
};

void closeApp(GtkWidget *window, gpointer data)
{
    gtk_main_quit();
}

R05_DEFINE_ENTRY_FUNCTION(TreeView, "TreeView") {
  struct r05_node *callee = arg_begin->next;

  if (callee->next != arg_end)
    r05_recognition_impossible();

  r05_reset_allocator();

  GtkWidget *window;
    GtkTreeStore *store;
    GtkWidget *view;
    GtkTreeIter parent_iter, child_iter;
    GtkCellRenderer *renderer;
    
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Tree");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(closeApp), NULL);

    store = gtk_tree_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    gtk_tree_store_append(store, &parent_iter, NULL);
    gtk_tree_store_set(store, &parent_iter,
                       COLUMN_TITLE, "Dark Side of the Moon",
                       COLUMN_ARTIST, "Pink Floyd",
                       COLUMN_CATALOGUE, "B000024D4P",
                       -1);
    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COLUMN_TITLE, "Speak to Me", -1);

    view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                COLUMN_TITLE,
                                                "Title", renderer,
                                                "text", COLUMN_TITLE,
                                                NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                COLUMN_ARTIST,
                                                "Catalogue", renderer,
                                                "text", COLUMN_ARTIST,
                                                NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                COLUMN_CATALOGUE,
                                                "Catalogue", renderer,
                                                "text", COLUMN_CATALOGUE,
                                                NULL);

    gtk_container_add(GTK_CONTAINER(window), view);

    gtk_widget_show_all(window);

    gtk_main();

  r05_splice_from_freelist(arg_begin);
  r05_splice_to_freelist(arg_begin, arg_end);
}