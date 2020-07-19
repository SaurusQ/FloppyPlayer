
#ifndef MAIN_WND_HPP
#define MAIN_WND_HPP

#include <gtkmm.h>
#include <iostream>

class MainWnd : public Gtk::Window
{
    public:
        MainWnd();
        ~MainWnd();

        void playButton_clicked() { std::cout << "play" << std::endl; }
        void forwardButton_clicked() { std::cout << "forw" << std::endl; }
        void backwardButton_clicked() { std::cout << "back" << std::endl; }

        void time_scroll() { std::cout << "time scroll: " << timeScale_.get_value() << std::endl; }

        void file_chosen() { std::cout << "file chosen: " << fileChooser_.get_uri() << std::endl; }

    private:
        Gtk::Grid               grid_               = Gtk::Grid();
        Gtk::Button             playButton_         = Gtk::Button();
        Gtk::Button             forwardButton_      = Gtk::Button();
        Gtk::Button             backwardButton_     = Gtk::Button();
        Gtk::Scale              timeScale_          = Gtk::Scale();
        Gtk::FileChooserButton  fileChooser_        = Gtk::FileChooserButton("Select folder", Gtk::FILE_CHOOSER_ACTION_OPEN);
        
        //Gtk::Image playImage_;
        //Gtk::ComboBoxText filesCombo_;
        //Gtk::ComboBoxText usbCombo_;
        //Gtk::ScrolledWindow floppySettingsWnd_;workbench.action.toggleActivityBarVisibility
        //Gtk::Separator separator_;
};

#endif
