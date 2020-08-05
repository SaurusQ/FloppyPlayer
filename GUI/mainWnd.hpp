
#ifndef MAIN_WND_HPP
#define MAIN_WND_HPP

#include <gtkmm.h>
#include <gtkmm/adjustment.h>
#include <iostream>
#include <future>

#include "midiPlayer.hpp"

class MainWnd : public Gtk::Window
{
    public:
        MainWnd();
        ~MainWnd();

        void playButton_clicked();
        void pauseButon_clicked();
        void forwardButton_clicked() { std::cout << "forw" << std::endl; }
        void backwardButton_clicked() { std::cout << "back" << std::endl; }
        void time_scroll() { std::cout << "time scroll: " << timeScale_.get_value() << std::endl; }
        void file_chosen();
        void spin_button();

    private:

        MidiPlayer player_;
        std::future<void> playFuture_;

        Gtk::Grid               grid_               = Gtk::Grid();
        Gtk::Button             playButton_         = Gtk::Button();
        Gtk::Button             forwardButton_      = Gtk::Button();
        Gtk::Button             backwardButton_     = Gtk::Button();
        Gtk::Scale              timeScale_          = Gtk::Scale();
        Gtk::FileChooserButton  fileChooser_        = Gtk::FileChooserButton("Select folder", Gtk::FILE_CHOOSER_ACTION_OPEN);
        Glib::RefPtr<Gtk::Adjustment> refSpinButtonAdj_ = Gtk::Adjustment::create(0.0, -20.0, 20.0);
        Gtk::SpinButton         spinButton_         = Gtk::SpinButton(refSpinButtonAdj_);

        //Gtk::Image playImage_;
        //Gtk::ComboBoxText filesCombo_;
        //Gtk::ComboBoxText usbCombo_;
        //Gtk::ScrolledWindow floppySettingsWnd_;workbench.action.toggleActivityBarVisibility
        //Gtk::Separator separator_;
};

#endif
