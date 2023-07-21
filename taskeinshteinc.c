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
  int message_len;

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

<TreeSolveView> ==

*/
enum {
    COLUMN_HOME,
    COLUMN_COLOR,
    COLUMN_NATIONALITY,
    COLUMN_CIGARETTES,
    COLUMN_ANIMAL,
    COLUMN_DRINK,
    N_COLUMNS
};

void closeWin(GtkWidget *window, gpointer data)
{
    gtk_main_quit();
}

GtkTreeStore *store;

R05_DEFINE_ENTRY_FUNCTION(TreeSolveView, "TreeSolveView") {
  struct r05_node *callee = arg_begin->next;
  
  if (callee->next != arg_end)
    r05_recognition_impossible();

  r05_reset_allocator();
  
  GtkWidget *window;
  GtkWidget *scrolled_window;
  GtkWidget *view;
  GtkCellRenderer *renderer;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Решение задачи Эйнштейна");
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 930, 725);

  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(closeWin), NULL);

  scrolled_window = gtk_scrolled_window_new(NULL, NULL);

  view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                              -1,
                                              "Дом", renderer,
                                              "text", COLUMN_HOME,
                                              NULL);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                              -1,
                                              "Цвет", renderer,
                                              "text", COLUMN_COLOR, 
                                              NULL);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                              -1,
                                              "Национальность", renderer,
                                              "text", COLUMN_NATIONALITY,
                                              NULL);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                              -1,
                                              "Сигареты", renderer,
                                              "text", COLUMN_CIGARETTES,
                                              NULL);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                              -1,
                                              "Животное", renderer,
                                              "text", COLUMN_ANIMAL, 
                                              NULL);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                              -1,
                                              "Напиток", renderer,
                                              "text", COLUMN_DRINK,
                                              NULL);
  
  
  gtk_tree_view_expand_all(GTK_TREE_VIEW(view));

  gtk_container_add(GTK_CONTAINER(scrolled_window), view);
  gtk_container_add(GTK_CONTAINER(window), scrolled_window);
  
  gtk_widget_show_all(window);

  gtk_main();

  r05_splice_from_freelist(arg_begin);
  r05_splice_to_freelist(arg_begin, arg_end);
}

/*

<TreeSolveNew> ==

*/
R05_DEFINE_ENTRY_FUNCTION(TreeSolveNew, "TreeSolveNew") {
  struct r05_node *callee = arg_begin->next;

  if (callee->next != arg_end)
    r05_recognition_impossible();

  r05_reset_allocator();

  store = gtk_tree_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

  r05_splice_from_freelist(arg_begin);
  r05_splice_to_freelist(arg_begin, arg_end);
}

/*

<TreeSolveRule (e.Rule) e.RuleText> ==

e.Rule ::= 'Подсказка ' s.Rule
s.Rule ::= s.NUMBER
e.RuleText ::= 'Англичанин живет в красном доме.' | 'У шведа есть собака.' |
  | 'Датчанин пьет чай.' | 'Зеленый дом стоит слева от белого.' | 'Обитатель зеленого дома пьет кофе.' |
  | 'Человек, который курит Pall Mall, держит птицу.' | 'Обитатель среднего дома пьет молоко.' |
  | 'Норвежец живет в первом доме.' | 'Курильщик Marlboro живет возле того, у кого есть кошка.' |
  | 'Человек, у которого есть конь, живет возле того, кто курит Dunhill.' | 'Курильщик Winfield пьет пиво.' |
  | 'Норвежец живет возле голубого дома.' | 'Немец курит Rothmans.' | 
  | 'Курильщик Marlboro живет по соседству с человеком, который пьет воду.' | 
  | 'Условия:'
    '1. Есть 5 домов разных цветов.'
    '2. В каждом доме живет по одному человеку, отличающегося от другого по национальности.'
    '3. Каждый обитатель пьет только один определенный напиток, курит определенную марку сигарет и держит животное.'
    '4. Никто из пяти людей не пьет одинакового напитка, не курит одинаковые сигареты и не держит одинаковых животных.' |
  | 'Рыба - пятое животное.'

*/
GtkTreeIter parent_iter;
  
R05_DEFINE_ENTRY_FUNCTION(TreeSolveRule, "TreeSolveRule") {
  struct r05_node *callee = arg_begin->next;

  /* (e.Rule) */
  struct r05_node *trl_b = callee->next, *trl_e;
  
  if (trl_b == arg_end)
    r05_recognition_impossible(); 

  if (R05_DATATAG_OPEN_BRACKET != trl_b->tag)
    r05_recognition_impossible();
  
  trl_e = trl_b->info.link;

  struct r05_node *rl_b, *rl_e;

#define RULE_MAX 64
  char rule[RULE_MAX + 1];
  int rule_len;

  r05_prepare_argument(&rl_b, &rl_e, callee, trl_e);
  rule_len = r05_read_chars(rule, RULE_MAX, &rl_b, &rl_e);
  rule[rule_len] = '\0';

  if (rule_len == 0)
    r05_recognition_impossible();

  if (!r05_empty_seq(rl_b, rl_e))
  {
    struct r05_node *p = rl_b;
    
    while (p != rl_e->next && p->tag == R05_DATATAG_CHAR)
      p = p->next;

    if (p == rl_e->next)
      r05_builtin_error("very long rule");
    else
      r05_recognition_impossible();
  }

#undef RULE_MAX

  /* e.RuleText */
  struct r05_node *rlt_b, *rlt_e;

#define RULETEXT_MAX 1024
  char ruletext[RULETEXT_MAX + 1];
  int ruletext_len;

  r05_prepare_argument(&rlt_b, &rlt_e, trl_e->prev, arg_end);
  ruletext_len = r05_read_chars(ruletext, RULETEXT_MAX, &rlt_b, &rlt_e);
  ruletext[ruletext_len] = '\0';

  if (ruletext_len == 0)
    r05_recognition_impossible();

  if (!r05_empty_seq(rlt_b, rlt_e))
  {
    struct r05_node *p = rlt_b;
    
    while (p != rlt_e->next && p->tag == R05_DATATAG_CHAR)
      p = p->next;

    if (p == rlt_e->next)
      r05_builtin_error("very long ruletext");
    else
      r05_recognition_impossible();
  }

#undef RULETEXT_MAX

  r05_reset_allocator();

  gtk_tree_store_append(store, &parent_iter, NULL);
  gtk_tree_store_set(store, &parent_iter,
                    COLUMN_HOME, rule,
                    COLUMN_COLOR, ruletext,
                    -1);

  r05_splice_from_freelist(arg_begin);
  r05_splice_to_freelist(arg_begin, arg_end);
}

/*

<TreeSolveTable e.TableHome> ==

e.TableHome ::= s.Home (e.Color) (e.Nationality) (e.Cigarettes) (e.Animal) e.Drink
s.Home ::= '1'..'5'
e.Color ::= 'Жёлтый' | 'Голубой' | 'Красный' | 'Зелёный' | 'Белый' | '???'
e.Nationality ::= 'Норвежец' | 'Датчанин'  |  'Англичанин' | 'Немец' | 'Швед' | '???'
e.Cigarettes ::= 'Dunhill' | 'Rothmans'  |  'Pall Mall' | 'Marlboro' | 'Winfield' | '???'
e.Animal ::= 'Кошка' | 'Конь'  |  'Птица' | 'Рыба' | 'Собака' | '???'
e.Drink ::= 'Вода' | 'Чай'  |  'Молоко' | 'Кофе' | 'Пиво' | '???'

*/
GtkTreeIter child_iter;
  
R05_DEFINE_ENTRY_FUNCTION(TreeSolveTable, "TreeSolveTable") {
  struct r05_node *callee = arg_begin->next;

   /* s.Home */
  struct r05_node *shm = callee->next;
  
  if (shm == arg_end)
    r05_recognition_impossible(); 

  if (R05_DATATAG_CHAR != shm->tag)
    r05_recognition_impossible();
  
  char home[2];
  home[0] = shm->info.char_;
  home[1] = '\0';
  
  /* (e.Color) (e.Nationality) (e.Cigarettes) (e.Animal) */
  struct r05_node *tobj_b = shm->next, *tobj_e;
  struct r05_node *obj_b, *obj_e;

#define OBJECT_MAX 64
  char object[5][OBJECT_MAX + 1];
  int object_len[5];
  
  for (int i = 0; i < 4; i++)
  {
    if (tobj_b == arg_end)
     r05_recognition_impossible(); 

    if (R05_DATATAG_OPEN_BRACKET != tobj_b->tag)
      r05_recognition_impossible();
  
    tobj_e = tobj_b->info.link;

    r05_prepare_argument(&obj_b, &obj_e, tobj_b->prev, tobj_e);
    object_len[i] = r05_read_chars(object[i], OBJECT_MAX, &obj_b, &obj_e);
    object[i][object_len[i]] = '\0';

    if (object_len[i] == 0)
      r05_recognition_impossible();

    if (!r05_empty_seq(obj_b, obj_e))
    {
      struct r05_node *p = obj_b;
    
      while (p != obj_e->next && p->tag == R05_DATATAG_CHAR)
        p = p->next;

      if (p == obj_e->next)
        r05_builtin_error("very long objectname");
      else
        r05_recognition_impossible();
    }

    tobj_b = tobj_e->next;
  }
  
  /* e.Drink */

  r05_prepare_argument(&obj_b, &obj_e, tobj_e->prev, arg_end);
  object_len[4] = r05_read_chars(object[4], OBJECT_MAX, &obj_b, &obj_e);
  object[4][object_len[4]] = '\0';

  if (object_len[4] == 0)
    r05_recognition_impossible();

  if (!r05_empty_seq(obj_b, obj_e))
  {
    struct r05_node *p = obj_b;
    
    while (p != obj_e->next && p->tag == R05_DATATAG_CHAR)
      p = p->next;

    if (p == obj_e->next)
      r05_builtin_error("very long objectname");
    else
      r05_recognition_impossible();
  }

#undef OBJECT_MAX

  r05_reset_allocator();

  gtk_tree_store_append(store, &child_iter, &parent_iter);
  gtk_tree_store_set(store, &child_iter, 
                    COLUMN_HOME, home,
                    COLUMN_COLOR, object[0],
                    COLUMN_NATIONALITY, object[1],
                    COLUMN_CIGARETTES, object[2],
                    COLUMN_ANIMAL, object[3],
                    COLUMN_DRINK, object[4],
                    -1);

  r05_splice_from_freelist(arg_begin);
  r05_splice_to_freelist(arg_begin, arg_end);
}