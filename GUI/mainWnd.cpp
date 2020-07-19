
#include "mainWnd.hpp"

MainWnd::MainWnd()
{

    this->set_border_width(10);
    this->set_title("Floppy Player");
    this->set_default_size(600,400);

    // buttons
    //playImage_ = Gtk::Image("./assets/play.svg");
    //playImage_.set_pixel_size(0.1);
    //playButton_.set_image(playImage_);
    //playButton_.set_always_show_image();
    playButton_.set_label("Play");
    forwardButton_.set_label("forw");
    backwardButton_.set_label("back");
    playButton_.signal_pressed().connect(sigc::mem_fun(*this, &MainWnd::playButton_clicked));
    forwardButton_.signal_pressed().connect(sigc::mem_fun(*this, &MainWnd::forwardButton_clicked));
    backwardButton_.signal_pressed().connect(sigc::mem_fun(*this, &MainWnd::backwardButton_clicked));

    // scale
    timeScale_.set_range(0, 100);
    timeScale_.set_value(0);
    timeScale_.signal_value_changed().connect(sigc::mem_fun(*this, &MainWnd::time_scroll));

    // file chooser
    fileChooser_.set_title("Select midi file");
    // filter
    Glib::RefPtr<Gtk::FileFilter> filterMidi = Gtk::FileFilter::create();
    filterMidi->set_name("midi files");
    filterMidi->add_pattern("*.mid");
    fileChooser_.add_filter(filterMidi);
    fileChooser_.signal_file_set().connect(sigc::mem_fun(*this, &MainWnd::file_chosen));
    //fileChooser_.signal_file_set().connect([](Gtk::FileChooserButton button) {MainWnd::get_file(button);});
    //fileChooser_.set_
    //filesCombo_ = Gtk::ComboBoxText();
    //usbCombo_ = Gtk::ComboBoxText(true);
    //usbCombo_.set_entry_text_column()

    //separator_ = Gtk::Separator();

    //floppySettingsWnd_ = Gtk::ScrolledWindow();

    grid_  = Gtk::Grid();
    grid_.attach(backwardButton_,       0, 0, 1, 1);
    grid_.attach(playButton_,           1, 0, 1, 1);
    grid_.attach(forwardButton_,        2, 0, 1, 1);
    grid_.attach(timeScale_,            0, 1, 5, 1);
    grid_.attach(fileChooser_,          0, 2, 4, 4);
    
    this->add(grid_);
    this->show_all_children();
}

MainWnd::~MainWnd()
{

}
