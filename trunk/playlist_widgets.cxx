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

#include <cstdlib>
#include <cassert>

#include <fstream>

#include "playlist_widgets.h"

/*** class PlaylistColumns ***/
/*
template <class T>
PlaylistColumns<T>::PlaylistColumns()
    //~ :name("Name"), genre("Genre")
{
    //~ add(c_name);
    this->Gtk::TreeModel::ColumnRecord::add(id_column);
}

template <class T>
void
PlaylistColumns<T>::add(const PlaylistColumns<T>::key_type& name,
			PlaylistColumns<T>::column_type& column){
    column_ordering.push_back(name);
    column_mapping[name] = &column;
    this->Gtk::TreeModel::ColumnRecord::add(column);
}

template <class T>
const std::vector<typename PlaylistColumns<T>::key_type>&
PlaylistColumns<T>::get_column_names() const {
    return column_ordering;
}

//~ template <class T>
//~ typename PlaylistColumns<T>::id_column_type&
//~ PlaylistColumns<T>::get_id_column(){ return id_column; }

template <class T>
typename PlaylistColumns<T>::column_type&
PlaylistColumns<T>::get_column(const PlaylistColumns<T>::key_type& name){
    // FIXME: what if name is not a key? then a new column is created, not what we want, use exceptions?
    return *column_mapping[name];
}
*/

/*** class PlaylistView ***/
PlaylistView::PlaylistView()
	:Gtk::TreeView(),
	 m_NoStationsErrorDialog("This station has no channels, please choose another.")
{
    //~ set_column_types(m_Columns);
    
    // set the signal handler for activating a tree row
    //~ signal_row_activated().connect(sigc::mem_fun(m_PlaylistView, &PlaylistView::on_row_activated));
    dout(9) << "Setting dummy model" << std::endl;
    Gtk::TreeModel::ColumnRecord dummy_cols;
    Gtk::TreeModelColumn<Glib::ustring> dummy_col;
    dummy_cols.add(dummy_col);
    set_model(Gtk::ListStore::create(dummy_cols));
}

void PlaylistView::set_model(const Glib::RefPtr<Gtk::TreeModel>& model){
    Gtk::TreeView::set_model(model);
    
    //~ m_PlaylistView.set_expander_column(*m_PlaylistView.get_column(1));
    set_headers_clickable();
}

//~ static void create(){
    //~ this->Gtk::ListStore::create(m_Columns);
    //~ return Glib::RefPtr<PlaylistListStore>(this);
//~ }

//~ void PlaylistView::reset_columns(){}

#if 0
void PlaylistView::on_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column){
    dout(7) << "PlaylistView::on_row_activated()" << std::endl;
    std::cout << "  " << column->get_title() << ": " << path.to_string() << std::endl;
    std::cout << "  Playing MPlayer ..." << std::endl;
    
    // get the playstring
    Gtk::TreeModel::iterator iter = get_model()->get_iter(path);
    std::cout << "  activated row: " << path.to_string() << std::endl;
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
	std::cout << "  _id = " << id << " (" << 0 << ")" << std::endl;
	
	iter->get_value(get_model()->get_n_columns()-1, playstring);
	std::cout << "  playstring = " << playstring << " (" << get_model()->get_n_columns()-1 << ")" << std::endl;
	
	Glib::ustring body;
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
	    //~ cout << body_memory_trait.length() << ":||" << body << "||" << std::endl;
	    //~ cout << header_memory_trait.length() << ":||" << header << "||" << std::endl;
	}
	catch ( curlpp::exception & e )
	{
	  std::cerr << "+++CURLPP::ERROR : " << e.what() << std::endl;
	}
	
	PlaylistFile plsfile(body);
	
	for(std::vector<PlaylistFile::PlaylistFileDesc>::const_iterator i = plsfile.get_files().begin(); i != plsfile.get_files().end(); i++){
	    std::cout << "=== Title: " << i->title << std::endl;
	    std::cout << "=== File: " << i->filename << std::endl;
	}
	
	Glib::ustring streamurl;
	if(plsfile.size() <= 0){
	    // not stations to choose from
	    int ret = m_NoStationsErrorDialog.run();
	    std::cout << ">>> Responseid = " << ret << std::endl;
	}else if(plsfile.size() == 1){
	    // just play the station
	    streamurl = plsfile.get_files()[0].filename;
	}else{
	    // popup dialogbox so user can chose a station
	    std::cout << "<<<Choose Station>>>" << std::endl;
	    //~ Gtk::Dialog station_selection_Dialog("Choose Station", true, true);
	    
	    int which = m_SelectStationSSDialog.run(plsfile.get_titles());
	    std::cout << ">>> Responseid = " << which << std::endl;
	    
	    assert(which > -1 && which < (int)plsfile.get_files().size());
	    std::cout << "---| Title: " << plsfile.get_files()[which].title << std::endl;
	    std::cout << "---| File: " << plsfile.get_files()[which].filename << std::endl;
	    
	    streamurl = plsfile.get_files()[which].filename;
	}
	
	// send to mplayer
	//~ std::cout << "mplayer -v -nocache '" << streamurl << "'" << std::endl;
	std::cout << string_subst(m_parent->get_prefs().get_player_cmd(), streamurl) << std::endl;
    }else
	std::cerr << "iterator not valid? hmm, something fishy" << std::endl;
    
    return Gtk::TreeView::on_row_activated(path, column);
}
#endif

bool PlaylistView::on_button_press_event(GdkEventButton *evbutton){
    dout(7) << "PlaylistView::on_button_press_event()" << std::endl;
    assert(evbutton->type == GDK_BUTTON_PRESS ||
	   evbutton->type == GDK_2BUTTON_PRESS ||
	   evbutton->type == GDK_3BUTTON_PRESS);
    if(evbutton->button == RIGHT_MOUSE_BUTTON){
	// need to check for right-click, that is right press -> right release
	dout(9) << "  Type: " << evbutton->type << std::endl;
	dout(9) << "  Button: " << evbutton->button << std::endl;
    }
    
    // call default handler
    return Gtk::TreeView::on_button_press_event(evbutton);
}

bool PlaylistView::on_button_release_event(GdkEventButton *evbutton){
    dout(7) << "PlaylistView::on_button_release_event()" << std::endl;
    assert(evbutton->type == GDK_BUTTON_RELEASE);
    if(evbutton->button == RIGHT_MOUSE_BUTTON){
	// need to check for right-click, that is right press -> right release
	dout(9) << "  Type: " << evbutton->type << std::endl;
	dout(9) << "  Button: " << evbutton->button << std::endl;
	
	int tx=0, ty=0;
	tree_to_widget_coords((int) evbutton->x, (int) evbutton->y,
			      tx, ty);
	std::cout << "  tx = " << tx << ", ty = " << ty << std::endl;
	
	int cellx=0, celly=0;
	Gtk::TreeModel::Path path;
	Gtk::TreeViewColumn* column;
	if(get_path_at_pos((int) evbutton->x, (int) evbutton->y, path, column, cellx, celly)){
	    std::cout << "  cellx = " << cellx << std::endl;
	    std::cout << "  celly = " << celly << std::endl;
	    std::cout << "  path = " << path.to_string() << std::endl;
	    std::cout << "  column = " << column->get_title() << std::endl;
	}
    }
    
    // call default handler
    return Gtk::TreeView::on_button_release_event(evbutton);
}
