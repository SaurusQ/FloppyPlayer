
#include "mainWnd.hpp"

MainWnd::MainWnd()
    : player_("COM3", 2)
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

    // spin button
    spinButton_.signal_changed().connect(sigc::mem_fun(*this, &MainWnd::spin_button));

    //separator_ = Gtk::Separator();

    //floppySettingsWnd_ = Gtk::ScrolledWindow();

    grid_  = Gtk::Grid();
    grid_.attach(backwardButton_,       0, 0, 1, 1);
    grid_.attach(playButton_,           1, 0, 1, 1);
    grid_.attach(forwardButton_,        2, 0, 1, 1);
    grid_.attach(timeScale_,            0, 1, 5, 1);
    grid_.attach(fileChooser_,          0, 2, 5, 1);
    grid_.attach(spinButton_,           0, 3, 5, 1);

    // track selection
    for(unsigned int i = 0; i < 16; i++)
    {
        for(unsigned int j = 0; j < 9; j++)
        {
            checkTrack_[i][j] = Gtk::CheckButton();
            checkTrack_[i][j].signal_clicked().connect(sigc::mem_fun(*this, &MainWnd::check_track));
            trackGrid_.attach(checkTrack_[i][j], i, j, 1, 1);
        }
    }
    grid_.attach(trackGrid_, 0, 4, 5, 1);

    this->add(grid_);
    this->show_all_children();
}

MainWnd::~MainWnd()
{

}

void MainWnd::playButton_clicked() {
    player_.play();
    playButton_.set_label("pause");
    playButton_.signal_pressed().connect(sigc::mem_fun(*this, &MainWnd::pauseButon_clicked));
}

void MainWnd::pauseButon_clicked() {
    player_.pause();
    playButton_.set_label("play");
    playButton_.signal_pressed().connect(sigc::mem_fun(*this, &MainWnd::playButton_clicked));
}

void MainWnd::file_chosen()
{
    static bool first = true;
    std::cout << "file chosen: " << fileChooser_.get_uri() << std::endl;
    std::cout << "file test " << fileChooser_.get_filename() << std::endl;
    
    player_.endSong();

    if(first) first = false;
    else playFuture_.wait();
    
    playFuture_ = std::async([&]() {
        player_.parse(fileChooser_.get_filename());
        player_.playUSB();
    });
}

void MainWnd::spin_button()
{
    std::cout << "setting note diff to: " << spinButton_.get_value() << std::endl;
    player_.moveNotes(spinButton_.get_value());
}

void MainWnd::check_track()
{
    std::cout << "filter: ";
    std::vector<std::vector<bool>> filters(9, std::vector<bool>(16, false));

    for(unsigned int i = 0; i < 16; i++)
    {
        for(unsigned int j = 0; j < 6; j++)
        {
            std::cout << checkTrack_[i][j].get_active() << " ";
            filters[j][i] = checkTrack_[i][j].get_active();
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
    player_.setTrackFilter(filters);
}
