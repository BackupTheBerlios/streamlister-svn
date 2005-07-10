/* Copyright (C) 2004  crass <crass@users.berlios.de>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//~ #include <sys/types.h>
//~ #include <sys/stat.h>
//~ #include <unistd.h>  // system
//~ #include <fcntl.h>   // open

#include <cstdlib>
#include <cassert>

#include <iostream>
//~ #include <fstream>

#include <string>

#include <gtkmm.h>

#ifdef HAVE_LIBCURLPP_DEV
# include <curlpp/cURLpp.hpp>
# include <curlpp/Options.hpp>
# include <curlpp/Exception.hpp>
#else
# include <curlpp/curlpp.hpp>
# include <curlpp/http_easy.hpp>
#endif /* HAVE_LIBCURLPP_DEV */

//~ #include <boost/filesystem/operations.hpp>

#define _DEPRECIATED_FILESELECTION 0
#define __C_VIDEO_HACK_

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif

#include "configuration.h"
#include "dialog_boxes.h"
#include "playlist.h"
#include "playlist_widgets.h"
#include "utility.h"
#include "player_window.h"

/* globally constructed objects */
dstream dout;

class MainWindow : public Gtk::Window {
    public:
	MainWindow(int argc, char *argv[]);
	virtual ~MainWindow();
	
	void load_data();
	void reload();
	void search();
	void genre_filter();
    
    private:
	
	Gtk::Widget* _create_menu();
	Gtk::Widget* _create_topbar();
	Gtk::Widget* _create_liststore();
    
	void _populate_liststore();
	void _populate_genrefilter();
	void _reset_columns();
	void _create_columns();
	void _run_preferences();
	void _save_as();
	void _do_nothing();
    
	void _get_playlist();
	void _get_playlistfile(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
    
	void _make_tmp_filename();
    
	bool _playlist_filter(const Gtk::TreeModel::const_iterator& iter);
	bool _search_filter(const Gtk::TreeModel::const_iterator& iter);
	bool _genre_filter(const Gtk::TreeModel::const_iterator& iter);
	bool _rating_filter(const Gtk::TreeModel::const_iterator& iter);
    
    protected:
	
	//Member widgets:
	// Menu widgets:
	Gtk::Button m_ReloadButton;
	Gtk::VBox m_MainContentVBox;
	Glib::RefPtr<Gtk::UIManager> m_refUIManager;
	Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
	
	// Option widgets:
	Gtk::HBox OptionsHBox;
	std::vector <Gtk::VSeparator*> m_OptionSeparators;
	Gtk::Entry m_SearchEntry;
	Gtk::Button m_SearchButton;
	Gtk::Label m_GenreLabel;
	Gtk::ComboBoxText m_GenreCombo;
	
	// List widgets:
	Gtk::ScrolledWindow m_ScrolledWindow;
	PlaylistColumns<Glib::ustring> m_Columns;
	//~ Gtk::ListStore liststore;
	Glib::RefPtr<Gtk::TreeModelFilter> m_TreeModelFilterRef;
	Glib::RefPtr<Gtk::ListStore> m_ListStoreRef;
	//~ Gtk::TreeView m_PlaylistView;
	PlaylistView m_PlaylistView;
	//~ PlaylistView<MainWindow> m_PlaylistView;
	
	// Dialog boxes
	PreferencesDialog  m_PreferencesDialog;
	AboutDialog        m_AboutDialog;
	//~ Gtk::FileSelection m_FileSelection;
	//~ Gtk::FileChooserDialog m_FileChooserDialog;
	ErrorDialog            m_NoStationsErrorDialog;
	StationSelectionDialog m_SelectStationSSDialog;
	
	Playlist playlist_data;
	Glib::ustring m_current_genre_filter;
	Glib::ustring m_current_search_filter;
	Glib::ustring m_tmp_filename;
	//~ Glib::ustring m_shoucast_url;
	Glib::ustring m_player_cmd;
	
#ifdef HAVE_LIBCURLPP_DEV
	// cURLpp stuff
	cURLpp::Cleanup myCleanup;
#endif

};


MainWindow::MainWindow(int argc, char *argv[])
     :m_PreferencesDialog(DEFAULT_CONFIG_PATH),
      m_NoStationsErrorDialog("This station has no channels, please choose another.")
{
    // add option parsing stuff, need do have debug and help
    
#ifndef HAVE_LIBCURLPP_DEV
    // initialize curlpp
    curlpp::initialize();
#endif
    
    // Sets the border width of the window.
    set_title("streamlistings");
    set_border_width(5);
    set_default_size(700, 700); // asking
    set_size_request(300, 100); // commanding
    
    // get the stream listings
    _get_playlist();
    
    //~ _make_tmp_filename();
    //~ m_shoucast_url = Glib::ustring(SHOUTCAST_URL);
    
    // make columns
    for(std::vector<Glib::ustring>::const_iterator i = playlist_data.get_properties().begin(); i != playlist_data.get_properties().end(); i++){
	dout(9) << "adding column to colgroup: " << *i << std::endl;
	m_Columns.add(*i);
    }
    
    Gtk::Widget* pwMenu = _create_menu();
    Gtk::Widget* pwTopBar = _create_topbar();
    Gtk::Widget* pwPlaylist = _create_liststore();
    
    // add widgets
    if(pwMenu)
	m_MainContentVBox.pack_start(*pwMenu, Gtk::PACK_SHRINK);
    m_MainContentVBox.pack_start(*pwTopBar, Gtk::PACK_SHRINK, 3 /* padding */);
    m_MainContentVBox.add(*pwPlaylist);
    
    add(m_MainContentVBox);
    
    show_all_children();
    
    // load data into tree widget
    load_data();
}

MainWindow::~MainWindow(){
    dout(7) << "MainWindow::~MainWindow()" << std::endl;
#ifndef HAVE_LIBCURLPP_DEV
    // cleanup curlpp
    curlpp::terminate();
#endif
    
    // kill temp file if it exists
    //~ struct stat temp_buf;
    //~ if(stat(m_tmp_filename.c_str(), &temp_buf) != -1){
	//~ if(unlink(m_tmp_filename.c_str()) == -1){
	    //~ std::cerr << "Unable to unlink(): " << m_tmp_filename.c_str() << std::endl;
	//~ }
    //~ }
    // kill temp file if it exists
    if(file_exists(m_tmp_filename)){
	if(unlink(m_tmp_filename.c_str()) == -1){
	    std::cerr << "Unable to unlink(): " << m_tmp_filename.c_str() << std::endl;
	}
    }
    
    /* save prefs */
    m_PreferencesDialog.save();
}

void MainWindow::load_data(){
    // clear what ever was in the list store
    m_ListStoreRef->clear();
    _populate_liststore();
    
    // clear list first
    //+++ FIXME: gtkmm design bug, ComboBoxText needs to have remove_text()
    //  using gtk_combo_box_remove_text()
    
    //~ Glib::RefPtr<Gtk::TreeStore>(m_GenreCombo.get_model())->clear();
    Glib::RefPtr<Gtk::ListStore>::cast_static(m_GenreCombo.get_model())->clear();
    //~ while(m_GenreCombo.get_model()->get_n_columns() > 0)
	//~ gtk_combo_box_remove_text (m_GenreCombo.gobj(), 0)
    //~ m_GenreCombo.set_model(m_GenreCombo.get_model());
    _populate_genrefilter();
}

void MainWindow::reload(){
    dout(7) << "MainWindow::reload()" << std::endl;
    // get the stream listings
    _get_playlist();
    load_data();
}

void MainWindow::search(){
    dout(7) << "MainWindow::search()" << std::endl;
    m_current_search_filter = m_SearchEntry.get_text();
    
    m_TreeModelFilterRef->refilter();
}

void MainWindow::genre_filter(){
    dout(7) << "MainWindow::genre_filter()" << std::endl;
    dout(9) << "  Active Row Num: " << m_GenreCombo.get_active_row_number() << std::endl;
    if(m_GenreCombo.get_active_row_number() > -1){
	Glib::ustring data("");
	m_GenreCombo.get_model()->children()[m_GenreCombo.get_active_row_number()].get_value(0, data);
	dout(9) << "  Selected: " << *(m_GenreCombo.get_active()) << " : " << data << std::endl;
	
	// start filtering rows
	m_current_genre_filter = data;
	m_TreeModelFilterRef->refilter();
    }
}

/****                                                                                    ****/
/**** START PRIVATE MEMBER FUNCTIONS  ****/
/****                                                                                    ****/
Gtk::Widget* MainWindow::_create_menu(){
    dout(7) << "MainWindow::_create_menu()" << std::endl;
    //Create actions for menus and toolbars:
    m_refActionGroup = Gtk::ActionGroup::create();
    
    //File|New sub menu:
    m_refActionGroup->add( Gtk::Action::create("FileNewStandard", Gtk::Stock::NEW, "_New", "Create a new file"),
	sigc::mem_fun(*this, &MainWindow::_do_nothing) );
    
    m_refActionGroup->add( Gtk::Action::create("FileNewFoo", Gtk::Stock::NEW, "New Foo", "Create a new foo"),
	sigc::mem_fun(*this, &MainWindow::_do_nothing) );
    
    m_refActionGroup->add( Gtk::Action::create("FileNewGoo", Gtk::Stock::NEW, "_New Goo", "Create a new goo"),
	sigc::mem_fun(*this, &MainWindow::_do_nothing) );
    
    //File menu:
    m_refActionGroup->add( Gtk::Action::create("FileMenu", "File") );
    m_refActionGroup->add( Gtk::Action::create("FileNew", Gtk::Stock::NEW) ); //Sub-menu.
    m_refActionGroup->add( Gtk::Action::create("FileSavePrefs", Gtk::Stock::SAVE, "_Save Prefs"),
	sigc::mem_fun(m_PreferencesDialog, &PreferencesDialog::save) );
    m_refActionGroup->add( Gtk::Action::create("FileSaveAs", Gtk::Stock::SAVE_AS),
	Gtk::AccelKey::AccelKey('s', Gdk::CONTROL_MASK | Gdk::SHIFT_MASK),
	sigc::mem_fun(*this, &MainWindow::_save_as) );
    m_refActionGroup->add( Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
	sigc::mem_fun(*this, &MainWindow::hide) );
	//~ Gtk::Main::quit );
    
    //Edit menu:
    m_refActionGroup->add( Gtk::Action::create("EditMenu", "Edit") );
    m_refActionGroup->add( Gtk::Action::create("EditPreferences", Gtk::Stock::PREFERENCES, "_Preferences"),
	sigc::mem_fun(*this, &MainWindow::_run_preferences) );
    
    //Columns menu:
    //~ m_refActionGroup->add( Gtk::Action::create("ColumnsMenu", "Columns") );
    //~ for(std::vector<Glib::ustring>::const_iterator i = playlist_data.get_properties().begin(); i != playlist_data.get_properties().end(); i++){
	//~ dout(9) << "adding action to actiongroup: " << *i << std::endl;
	//~ m_refActionGroup->add( Gtk::ToggleAction::create("Columns" + (*i), *i, Glib::ustring(), true),
	    //~ sigc::mem_fun(*this, &MainWindow::_reset_columns) );
    //~ }
    
    //Ratings menu:
    m_refActionGroup->add( Gtk::Action::create("RatingsMenu", "Ratings") );
    
    //Bookmark menu:
    m_refActionGroup->add( Gtk::Action::create("BookmarksMenu", "Bookmarks") );
    //~ m_refActionGroup->add( Gtk::Action::create("EditSomething", "Something"),
	//~ sigc::mem_fun(*this, &MainWindow::_do_nothing) );
    
    //Help menu:
    m_refActionGroup->add( Gtk::Action::create("HelpMenu", "Help") );
    //~ m_refActionGroup->add( Gtk::Action::create("HelpAbout", Gtk::Stock::HELP),
	//~ sigc::mem_fun(*this, &MainWindow::_do_nothing) );
    m_refActionGroup->add( Gtk::Action::create("HelpAbout", "About"),
	sigc::hide_return(sigc::mem_fun(m_AboutDialog, &AboutDialog::run)) );
    
    m_refUIManager = Gtk::UIManager::create();
    m_refUIManager->insert_action_group(m_refActionGroup);
    
    add_accel_group(m_refUIManager->get_accel_group());
    
    //Layout the actions in a menubar and toolbar:
    try
    {
	Glib::ustring ui_info_begin = 
	    "<ui>"
	    "  <menubar name='MenuBar'>"
	    "    <menu action='FileMenu'>"
	    "<!--      <menu action='FileNew'>"
	    "        <menuitem action='FileNewStandard'/>"
	    "        <menuitem action='FileNewFoo'/>"
	    "        <menuitem action='FileNewGoo'/>"
	    "      </menu> -->"
	    "      <separator/>"
	    "      <menuitem action='FileSavePrefs'/>"
	    "      <menuitem action='FileSaveAs'/>"
	    "      <menuitem action='FileQuit'/>"
	    "    </menu>"
	    "    <menu action='EditMenu'>"
	    "      <menuitem action='EditPreferences'/>";
	    //~ "    </menu>"
	    //~ "    <menu action='ColumnsMenu'>";
	
	Glib::ustring ui_info_columns = ""; 
	//~ for(std::vector<Glib::ustring>::const_iterator i = playlist_data.get_properties().begin(); i != playlist_data.get_properties().end(); i++){
	    //~ dout(9) << "adding action to ui: " << *i << std::endl;
	    //~ ui_info_columns += ("      <menuitem action='Columns" + (*i) + "'/>");
	//~ }
	
	Glib::ustring ui_info_end = 
	    "    </menu>"
	    "    <!-- <menuitem action='ReloadMenu'/> -->"
	    "    <menu action='BookmarksMenu'>"
	    "      <separator/>"
	    "      <menuitem action='HelpAbout'/>"
	    "    </menu>"
	    "    <menu action='HelpMenu'>"
	    "      <menuitem action='HelpAbout'/>"
	    "    </menu>"
	    "  </menubar>"
	    "  <toolbar  name='ToolBar'>"
	    "    <toolitem action='FileNewStandard'/>"
	    "    <toolitem action='FileQuit'/>"
	    "  </toolbar>"
	    "</ui>";
	
	
	Glib::ustring ui_info(ui_info_begin+ui_info_columns+ui_info_end);
	
	m_refUIManager->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex)
    {
	std::cerr << "building menus failed: " <<  ex.what();
    }

    
    //Get the menubar and toolbar widgets, and add them to a container widget:
    Gtk::Widget* pMenubar = m_refUIManager->get_widget("/MenuBar");
    
    // don't want toolbar
    //~ Gtk::Widget* pToolbar = m_refUIManager->get_widget("/ToolBar") ;
    //~ if(pToolbar)
	//~ m_Box.pack_start(*pToolbar, Gtk::PACK_SHRINK);
    
    show_all_children();
    
    return pMenubar;
}

Gtk::Widget* MainWindow::_create_topbar(){
    dout(7) << "MainWindow::_create_topbar()" << std::endl;
    // reload button
    m_ReloadButton.set_label(std::string("_Reload"));
    m_ReloadButton.set_use_underline();
    m_ReloadButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::reload));
    //~ m_ReloadButton.set_size_request(100,50);
    m_OptionSeparators.push_back(new Gtk::VSeparator);
    
    // 
    m_OptionSeparators.push_back(new Gtk::VSeparator);
    m_SearchButton.set_label(std::string("_Search"));
    m_SearchButton.set_use_underline();
    m_SearchButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::search));
    
    // set the values for the genres in the combobox
    m_GenreLabel.set_text(std::string("Genre Filter: "));
    m_GenreCombo.signal_changed().connect(sigc::mem_fun(*this, &MainWindow::genre_filter));
    m_OptionSeparators.push_back(new Gtk::VSeparator);
    
    std::vector<Gtk::VSeparator*>::iterator i_vs = m_OptionSeparators.begin();
    
    // add widgets
    //~ OptionsHBox.add(m_ReloadButton);
    OptionsHBox.pack_start(m_ReloadButton, Gtk::PACK_SHRINK);
    OptionsHBox.pack_start(**i_vs, Gtk::PACK_SHRINK, 7);
    
    OptionsHBox.pack_start(m_SearchEntry, Gtk::PACK_SHRINK);
    OptionsHBox.pack_start(m_SearchButton, Gtk::PACK_SHRINK);
    OptionsHBox.pack_start(**(++i_vs), Gtk::PACK_SHRINK, 7);
    
    OptionsHBox.pack_start(m_GenreLabel, Gtk::PACK_SHRINK);
    OptionsHBox.pack_start(m_GenreCombo, Gtk::PACK_SHRINK);
    OptionsHBox.pack_start(**(++i_vs), Gtk::PACK_SHRINK, 7);
    
    return &OptionsHBox;
}

Gtk::Widget* MainWindow::_create_liststore(){
    dout(7) << "MainWindow::_create_liststore()" << std::endl;
    
    // set columns in liststore
    m_ListStoreRef = Gtk::ListStore::create(m_Columns);
    m_TreeModelFilterRef = Gtk::TreeModelFilter::create(m_ListStoreRef, Gtk::TreeModel::Path() );
    m_TreeModelFilterRef->set_visible_func(sigc::mem_fun(*this, &MainWindow::_playlist_filter));
    
    //~ std::cout << "GTK_IS_TREE_VIEW (m_PlaylistView.gobj()) = " << GTK_IS_TREE_VIEW (m_PlaylistView.gobj()) << std::endl;
    
    // make treeview
    m_PlaylistView.set_model(m_TreeModelFilterRef);
    
    _create_columns();
    
    // connect signal for when the row is activated
    m_PlaylistView.signal_row_activated().connect(sigc::mem_fun(*this, &MainWindow::_get_playlistfile));
    
    //Only show the scrollbars when they are necessary:
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    m_ScrolledWindow.add(m_PlaylistView);
    
    // should be scrolled all the way left
    // THIS doesn't work
    //   For some reason the scrollbars seem to be postitioned correctly on start
    
    //~ m_ScrolledWindow.get_hadjustment()->set_value(0.0);
    //~ m_ScrolledWindow.get_hadjustment()->changed();
    //~ m_ScrolledWindow.get_vadjustment()->set_value(0.0);
    //~ m_ScrolledWindow.get_vadjustment()->changed();
    
    // show widgets
    m_PlaylistView.show();
    m_ScrolledWindow.show();
    
    return &m_ScrolledWindow;
}

void MainWindow::_populate_liststore(){
    std::vector<PlaylistEntry>::iterator i = playlist_data.get_entries().begin();
    for(int id=0; i != playlist_data.get_entries().end(); i++, id++){
	Gtk::TreeModel::iterator iter = m_ListStoreRef->append();
	
	// append an id column
	(*iter)[m_Columns.get_id_column()] = id;
	for(std::vector<Glib::ustring>::const_iterator j = m_Columns.get_column_names().begin(); j !=  m_Columns.get_column_names().end(); j++){
	    (*iter)[m_Columns.get_column(*j)] = i->get_data(*j);
	}
    }
}

void MainWindow::_populate_genrefilter(){
    m_GenreCombo.append_text(Glib::ustring("ALL"));
    for(std::vector<Glib::ustring>::const_iterator i = playlist_data.get_genres().begin();
	i != playlist_data.get_genres().end(); i++){
	dout(9) << "Adding genre to genre filter: " << *i << std::endl;
	m_GenreCombo.append_text(*i);
    }
    //~ m_GenreCombo.append_text(std::string("Last Genre"));
    m_GenreCombo.set_active(0);
}

void MainWindow::_reset_columns(){
    dout(7) << "MainWindow::_reset_columns" << std::endl;
    m_PlaylistView.remove_all_columns();
    
    _create_columns();
}

void MainWindow::_create_columns(){
    dout(7) << "MainWindow::_create_columns()" << std::endl;
    
    // make columns visible
    Glib::ListHandle<Glib::RefPtr<Gtk::Action> > m_ColumnsActions = m_refActionGroup->get_actions();
    
    // Goes through all actions and checks that way: BAD!
    //~ std::cout << "MainWindow::_reset_columns(): removed columns, now setting them" << std::endl;
    //~ for(Glib::ListHandle<Glib::RefPtr<Gtk::Action> >::iterator i = m_ColumnsActions.begin(); i != m_ColumnsActions.end(); i++){
	//~ if((*i)->get_name().compare(0, 7, "Columns") == 0 &&
		//~ (*i)->get_name() != "ColumnsMenu" &&
		//~ (Glib::RefPtr<Gtk::ToggleAction>::cast_static(*i))->get_active()){
	    //~ Glib::ustring columnName = (*i)->get_name().substr(7);
	    //~ std::cout << "appending column: " << columnName << std::endl;
	    //~ m_PlaylistView.append_column(columnName, m_Columns.get_column(columnName));
	//~ }
    //~ }
    
    for(std::vector<Glib::ustring>::const_iterator i = playlist_data.get_properties().begin(); i != playlist_data.get_properties().end(); i++){
    //~ for(Glib::ListHandle<Glib::RefPtr<Gtk::Action> >::iterator i = m_ColumnsActions.begin(); i != m_ColumnsActions.end(); i++){
	//~ Glib::RefPtr<Gtk::Action> refAction = m_refActionGroup->get_action(Glib::ustring("Columns") + (*i));
	//~ if(refAction && (Glib::RefPtr<Gtk::ToggleAction>::cast_static(refAction))->get_active()){
	Configuration::itemlist_type::const_iterator j = m_PreferencesDialog.get_columns().end();
	if((j = std::find(m_PreferencesDialog.get_columns().begin(), m_PreferencesDialog.get_columns().end(), std::make_pair(*i, true))) != m_PreferencesDialog.get_columns().end() && j->second){
	    //~ Glib::ustring columnName = (*i)->get_name().substr(7);
	    dout(9) << "appending column: " << (*i) << "(" << (&m_Columns.get_column(*i)) << ")" << std::endl;
	    m_PlaylistView.append_column(*i, m_Columns.get_column(*i));
	}
    }
    
    // make columns resizeable
    Glib::ListHandle<Gtk::TreeViewColumn*> m_ViewColumnList = m_PlaylistView.get_columns();
    for(Glib::ListHandle<Gtk::TreeViewColumn*>::iterator i = m_ViewColumnList.begin(); i != m_ViewColumnList.end(); i++){
	(*i)->set_resizable();
    }
}

void MainWindow::_run_preferences(){
    if(m_PreferencesDialog.run()){
	_reset_columns();
    }
    
    /* do changes made from preferences */
    //~ m_shoucast_url = m_PreferencesDialog.get_url();
}
    
void MainWindow::_save_as(){
    int run_ret = -1;
    // setup save as file chooser dialog
#ifdef _DEPRECIATED_FILESELECTION
    Gtk::FileSelection fcd("Save XML Listing As");
#else
    Gtk::FileChooserDialog fcd("Save XML Listing As", Gtk::FILE_CHOOSER_ACTION_SAVE);
    fcd.add_button("_Cancel", 0);
    fcd.add_button("_Ok", 1);
#endif
    
    if(run_ret = fcd.run()){
	dout(8) << "Chosen Filename: " << fcd.get_filename() << std::endl;
	dout(9) << "run_ret: " << run_ret << std::endl;
#ifdef _DEPRECIATED_FILESELECTION
	if(run_ret == -5) /* this sucks, -5 seems to == OK :/ */
#endif
	playlist_data.saveto_file(fcd.get_filename());
    }
}

void MainWindow::_do_nothing(){}

void MainWindow::_get_playlist(){
    dout(7) << "MainWindow::_get_playlist()" << std::endl;
    
    bool finvalid = false;
    Glib::ustring shoutcast_url = m_PreferencesDialog.get_url();
    
    /* make this configurable */
    dout(6) << "### m_shoucast_url = " << shoutcast_url << std::endl;
    size_t len = 0;
#ifdef HAVE_LIBCURLPP_DEV
    std::string buffer;
    try
    {
	cURLpp::Easy request;
	
	using namespace cURLpp::Options;
	request.setOpt(new Verbose(false));
	request.setOpt(new FollowLocation(false));
	request.setOpt(new NoBody(false));
	request.setOpt(new Header(false));
	request.setOpt(new UserAgent("User-Agent:Winamp/5.0"));
	request.setOpt(new HttpVersion(cURL::CURL_HTTP_VERSION_1_0));
	request.setOpt(new Url(shoutcast_url));
	
	request.setOpt(new WriteFunction(write_data));
	request.setOpt(new WriteData(&buffer));
	//~ cURLpp::Options::StorageOption<> body_memory_trait;
	//~ cURLpp::Options::StorageOption< cURLpp::cURL::curl_read_callback,
	
	//~ cURLpp::Options::StorageOption< sigc::slot<size_t, char*, size_t, size_t, void*>,
					//~ void *,
					//~ cURL::CURLOPT_READFUNCTION,
					//~ cURL::CURLOPT_READDATA > readCallback;
	//~ readCallback.setCallback(sigc::mem_fun(*this, &MainWindow::_read_data));
	//~ readCallback.setData(&buffer);
	//~ request.setOpt(readCallback);
	
	request.perform();
    }catch ( cURLpp::LogicError & e ){
       std::cout << e.what() << std::endl;
    }catch ( cURLpp::RuntimeError & e ){
       std::cout << e.what() << std::endl;
    }
    
    char *pbuffer = const_cast<char *>(buffer.data());
    len = buffer.length();
#else
    curlpp::memory_trait body_memory_trait;
    //~ do{
	try
	{
	    // grab new playlist
	    //~ std::ofstream file( m_tmp_filename.c_str() );
	    //~ curlpp::ostream_trait body_trait( &file );
	    curlpp::output_null_trait header_null_trait; // don't care about the headers
	    
	    curlpp::http_easy h_httpeasy;
	    h_httpeasy.verbose(false);
	    h_httpeasy.follow_location(false);
	    h_httpeasy.no_body(false);
	    h_httpeasy.header(false);
	    h_httpeasy.user_agent("User-Agent:Winamp/5.0");
	    h_httpeasy.http_version(curlpp::http_version::v1_0);
	    h_httpeasy.url(shoutcast_url);
	    
	    h_httpeasy.m_body_storage.trait(&body_memory_trait);
	    h_httpeasy.m_header_storage.trait(&header_null_trait);
	    
	    h_httpeasy.perform();
	}
	catch ( curlpp::exception & e )
	{
	    dout(3) << ("+++CURLPP::ERROR : "  __FILE__ ":") << __LINE__ << ": " << e.what() << std::endl;
	    //~ continue;
	}
	
	char *pbuffer = (char*)body_memory_trait.buffer();
	len = body_memory_trait.length();
#endif
	
#ifdef HAVE_LIBZ
	std::auto_ptr<char> ap;
	dout(1) << "Doing prelim check for xml format" << std::endl;
	// preliminary xml format check, if not try to inflate using zlib
	if(Glib::ustring(pbuffer, 5) != Glib::ustring("<?xml")){
	    dout(1) << "Not XML, trying to deflate ..." << std::endl;
	    
	    ap = decompress(pbuffer, len);
	    if(!ap.get()){
		dout(3) << "NULL decompression buffer returned (see above error)" << std::endl;
		return;
	    }
	    
	    dout(3) << "First 50 chars" << std::endl << Glib::ustring(ap.get(), 50) << std::endl;
	    
	    assert(Glib::ustring(ap.get(), 5) == Glib::ustring("<?xml"));
	    
	    pbuffer = ap.get();
	}
#endif /* HAVE_LIBZ */
	
	// parse xml tv listing
	try{
	    //~ playlist_data.parse_file(m_tmp_filename);
	    playlist_data.parse_memory(pbuffer);
	}catch(xmlpp::exception e){
	    //~ dout(3) << "ERROR parsing streamlisting: " << m_tmp_filename << std::endl;
	    dout(3) << "ERROR parsing streamlisting" << std::endl;
	    dout(9) << pbuffer << std::endl;
	    //~ continue;
	}catch(std::runtime_error e){
	    if(Glib::ustring(e.what()) == Glib::ustring("Incorrect Number of playlists")){
		dout(3) << "Incorrect Number of playlists" << std::endl;
		//~ continue;
	    }
	    throw;
	}
    //~ }while(finvalid);
}

void MainWindow::_get_playlistfile(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column){
    dout(7) << "MainWindow::_get_playlistfile()" << std::endl;
    dout(5) << "  " << column->get_title() << ": " << path.to_string() << std::endl;
    dout(5) << "  Playing MPlayer ..." << std::endl;
    
    // get the playstring
    Gtk::TreeModel::iterator iter = m_PlaylistView.get_model()->get_iter(path);
    dout(9) << "  activated row: " << path.to_string() << std::endl;
    if(iter){
	Glib::ustring playstring;
	int id = -1;
	// FIXME: this should be dynamic
	
	//~ for(int i=0; i < get_model()->get_n_columns(); i++){
	    //~ iter->get_value(i, playstring);
	    //~ std::cout << "  +name = " << playstring << std::endl;
	//~ }
	
	// get this id, so I can get which PlaylistEntry this is, so I can caache the results
	iter->get_value(0, id);
	dout(9) << "  _id = " << id << " (" << 0 << ")" << std::endl;
	
	iter->get_value(m_PlaylistView.get_model()->get_n_columns()-1, playstring);
	dout(8) << "  playstring = " << playstring << " (" << m_PlaylistView.get_model()->get_n_columns()-1 << ")" << std::endl;
	
	Glib::ustring body;
#ifdef HAVE_LIBCURLPP_DEV
	try
	{
	    cURLpp::Easy request;
	    
	    using namespace cURLpp::Options;
	    request.setOpt(new Verbose(false));
	    request.setOpt(new FollowLocation(false));
	    request.setOpt(new NoBody(false));
	    request.setOpt(new Header(false));
	    request.setOpt(new UserAgent("User-Agent:Winamp/5.0"));
	    request.setOpt(new HttpVersion(cURL::CURL_HTTP_VERSION_1_0));
	    request.setOpt(new Url(playstring));
	    
	    request.setOpt(new WriteFunction(write_data));
	    request.setOpt(new WriteData(&body));
	    
	    request.perform();
	}catch ( cURLpp::LogicError & e ){
	   std::cout << e.what() << std::endl;
	}catch ( cURLpp::RuntimeError & e ){
	   std::cout << e.what() << std::endl;
	}
#else
	try
	{
	    // get the real url
	    //~ curlpp::cleanup cleanup;
	    //~ curlpp::output_ustring_trait o_us_trait;
	    curlpp::memory_trait header_memory_trait;
	    curlpp::memory_trait body_memory_trait;
	    //~ std::ofstream file( "body.output" );
	    //~ curlpp::ostream_trait body_trait( &file );
	    
	    curlpp::http_easy h_httpeasy;
	    h_httpeasy.verbose(false);
	    h_httpeasy.follow_location(false);
	    h_httpeasy.no_body(false);
	    h_httpeasy.header(false);
	    h_httpeasy.user_agent("User-Agent:Winamp/5.0");
	    h_httpeasy.http_version(curlpp::http_version::v1_0);
	    h_httpeasy.url(playstring);
	    
	    //~ h_httpeasy.m_body_storage.trait(&o_us_trait);
	    //~ h_httpeasy.m_body_storage.trait(&body_trait);
	    h_httpeasy.m_body_storage.trait(&body_memory_trait);
	    h_httpeasy.m_header_storage.trait(&header_memory_trait);
	    
	    h_httpeasy.perform();
	    
	    body = body_memory_trait.string();
	    Glib::ustring header(header_memory_trait.string());
	    dout(9) << body_memory_trait.length() << ":||" << body << "||" << std::endl;
	    dout(9) << header_memory_trait.length() << ":||" << header << "||" << std::endl;
	}
	catch ( curlpp::exception & e )
	{
	  dout(3) << "+++CURLPP::ERROR : " << e.what() << std::endl;
	}
#endif
	
	PlaylistFile plsfile(body);
	
	for(std::vector<PlaylistFile::PlaylistFileDesc>::const_iterator i = plsfile.get_files().begin(); i != plsfile.get_files().end(); i++){
	    dout(5) << "=== Title: " << i->title << std::endl;
	    dout(5) << "=== File: " << i->filename << std::endl;
	}
	
	Glib::ustring streamurl;
	if(plsfile.size() <= 0){
	    // not stations to choose from
	    int ret = m_NoStationsErrorDialog.run();
	    dout(8) << ">>> Responseid = " << ret << std::endl;
	}else if(plsfile.size() == 1){
	    // just play the station
	    streamurl = plsfile.get_files()[0].filename;
	}else{
	    // popup dialogbox so user can chose a station
	    dout(8) << "<<<Choose Station>>>" << std::endl;
	    //~ Gtk::Dialog station_selection_Dialog("Choose Station", true, true);
	    
	    int which = m_SelectStationSSDialog.run(plsfile.get_titles());
	    dout(8) << ">>> Responseid = " << which << std::endl;
	    
	    assert(which > -1 && which < (int)plsfile.get_files().size());
	    dout(3) << "---| Title: " << plsfile.get_files()[which].title << std::endl;
	    dout(3) << "---| File: " << plsfile.get_files()[which].filename << std::endl;
	    
	    streamurl = plsfile.get_files()[which].filename;
	}
	
	// send to mplayer
	if(streamurl != "")
	    //~ new PlayerWindow(m_PreferencesDialog.get_player_cmd(), streamurl);
	    PlayerWindow(m_PreferencesDialog.get_player_cmd(), streamurl);
	else
	    dout(3) << "No stream url found." << std::endl;
	
	return;
	
#if 0
	//~ dout << "mplayer -v -nocache '" << streamurl << "'" << std::endl;
	Glib::ustring player_cmd = string_subst(m_PreferencesDialog.get_player_cmd(), streamurl);
	dout(3) << player_cmd << std::endl;
	//~ if(streamurl != "" && system(player_cmd.c_str()) == -1)
	    //~ std::cout << "system() errored" << std::endl;
	if(streamurl != ""){
#ifdef __C_VIDEO_HACK_
	    /* NOTE: it might be nice to do some command piping to MPlayer via
	     * -input command, but this wouldn't be general for all players */
	    
	    //~ char *args[] = {};
	    int stdout_fd = 1;
	    pid_t pid = 0;
	    pid = fork();
	    if(pid == 0){
		/* in child, do an exec */
		/* put us in our new session, 
		 * this should get rid of zombies */
		setsid();
		//~ setpgid(pid,0);
		
		/* FIXME:  do an execve (fuck it do it later) */
		/* hmm, I don't see this on my console, but for some reason I've seen on
		 * others where the app kills the shells stdout, but how can this do that?
		 * After a fork() the closing of copied fd's don't close the fd in the other process*/
		close(stdout_fd);
		/* make the outpupt descriptor point to null so it doesn't interfere with streamlister output */
		if(open("/dev/null", O_WRONLY) != stdout_fd){
		    dout(1) << "Error: open(\"/dev/null\", O_WRONLY) failed to open as stdout [" << stdout_fd << "]" << std::endl;
		    exit(0);
		}
		if(streamurl != "" && system(player_cmd.c_str()) == -1)
		    dout(1) << "system() errored" << std::endl;
		//~ dout(1) << "NEVER REACH HERE!!" << std::endl;
		exit(0);
	    }
#endif
	}
#endif
    }else
	dout(3) << "iterator not valid? hmm, something fishy" << std::endl;
}

void MainWindow::_make_tmp_filename(){
    pid_t mypid = getpid();
    // FIXME: get progname dynamically
    std::ostringstream oss;
    oss << Glib::ustring("/tmp/streamlister.") << mypid;
    m_tmp_filename = oss.str();
    dout(7) << "  m_tmp_filename = " << m_tmp_filename << std::endl;
}

bool MainWindow::_playlist_filter(const Gtk::TreeModel::const_iterator& iter){
    return _genre_filter(iter) && _search_filter(iter) && _rating_filter(iter);
}

bool MainWindow::_search_filter(const Gtk::TreeModel::const_iterator& iter){
    if(m_current_search_filter == "")
	return true;
    if(iter->get_value(m_Columns.get_column("Name")).find(m_current_search_filter) != std::string::npos)
	return true;
    return false;
}

bool MainWindow::_genre_filter(const Gtk::TreeModel::const_iterator& iter){
    // if filter == ALL then all rows visible
    if(m_current_genre_filter == "ALL")
	return true;
    if(m_current_genre_filter == iter->get_value(m_Columns.get_column("Genre")))
	return true;
    return false;
}

bool MainWindow::_rating_filter(const Gtk::TreeModel::const_iterator& iter){
    return true;
}

int main(int argc, char *argv[]){
    dout.set_debug_level(DEBUG_LEVEL);
    dout << "XML shoutcast tvlisting Parser" << std::endl;
    
    Gtk::Main kit(argc, argv);
    
    MainWindow mainWindow(argc, argv);
    
    Gtk::Main::run(mainWindow);
    
    return 0;
}
