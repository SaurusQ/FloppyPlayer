
#include <string.h>
#include <iostream>
#include <iomanip>
#include <windows.h>

#include "midiPlayer.hpp"

//Handling console(cursor)
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;
void setCursor(int x, int y)
{
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}


#include <gtk/gtk.h>

static void
print_hello (GtkWidget *widget,
             gpointer   data)
{
  g_print ("Hello World\n");
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *button_box;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_container_add (GTK_CONTAINER (window), button_box);

  button = gtk_button_new_with_label ("Hello World");
  g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
  gtk_container_add (GTK_CONTAINER (button_box), button);

  gtk_widget_show_all (window);
}

int main(int    argc,
      char **argv)
{
    GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;




    //The Pink Panther Theme.mid
    MidiPlayer test("S:\\Music\\Floppy Songs Audacity\\"
    //"Fallout - Mission; Imposible - Fallout (Piano Cover) - MIDI"
    //"DOOM (2016) - BFG Division"
    //"Doom - Level 9"
    //"Initial D - Running in the 90s"
    //"Star Wars The Force Awakens OST - March of The Resistance"//Real time category
    //"Undertale - Its Raining Somewhere Else" //Instrumental data
    //"Your_Name_-_Tate_no_Yuusha_no_Nariagari_ED"//Second track empty????
    //"Konosuba op fantastic dreamer"
    //"Pac-Man World - Ghostly Garden"
    //"Dragon Ball Super - Ultimate Battle"
    //"Full Metal Alchemist - Lapis Philosophorum"
    //"Pirates of the Caribbean - He's a Pirate (3)"
    //"Stardew Valley - Overture" //Nice
    //"Avengers- Age of Ultron - Heroes"
    //"Imagine Dragons - Believer"
    //"Dark Souls II - Menu Theme"
    //"Disney_Themes_-_DuckTales_2017_Full"
    //"Another_Colony_Piano_Version"
    "Nintendo_-_Pokemon_Fire_Red_Route_1_Piano_Cover_Hard_Version"
    //"Mega Man 2 - Quickman"
    //"Two Faced Lovers 19.4 Million"
    //"RISE_tvsize"
    //"The Pink Panther Theme"//Not working
    //"Rightfully"
    //"test"
    ".mid",
    "COM3");
    test.parse();
    test.printData();

    test.configurePlay();
    test.playUSB();
    


    while(1);
    return 0;
}
