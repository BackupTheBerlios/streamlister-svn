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

#ifndef __PLAYLIST_WIDGETS_H__
#define __PLAYLIST_WIDGETS_H__

#include <cstdlib>
#include <cassert>

#include <iostream>
#include <fstream>

#include <string>
#include <ext/hash_set>
#include <ext/hash_map>

#include <gtkmm.h>
#include <curlpp/curlpp.hpp>
#include <curlpp/http_easy.hpp>

#include "playlist.h"
#include "dialog_boxes.h"

#if 1
/*** curlpp trait derived classes  ***/
namespace curlpp
{
    class output_ustring_trait : public curlpp::output_trait {
	public:
	    output_ustring_trait( ) {}
	    virtual ~output_ustring_trait() {}
	    
	    virtual size_t write( void *buffer, size_t length ){
		std::cout << "--- output_ustring_trait::write()" << std::endl;
		m_data.append(static_cast<const char*>(buffer), length);
		return length;
	    }
	    
	private:
	    Glib::ustring m_data;
    };
    
    class output_null_trait : public curlpp::output_trait {
	public:
	    output_null_trait( ) {}
	    virtual ~output_null_trait() {}
	    
	    virtual size_t write( void *buffer, size_t length ){
		//~ std::cout << "--- output_null_trait::write()" << std::endl;
		return length;
	    }
    };
} /*** END CURLPP TRAITS ***/
#endif

/* 
 *  **** Start GTKmm code ****
*/
template <class T = Glib::ustring>
class PlaylistColumns : public Gtk::TreeModel::ColumnRecord{
    public:
	typedef T key_type;
	typedef Gtk::TreeModelColumn<key_type> column_type;
	typedef Gtk::TreeModelColumn<int> id_column_type;
	//~ typedef XXX iterator;

    public:
	PlaylistColumns();
    
	void add(const key_type&);
    
	const std::vector<key_type>& get_column_names() const;
    
	id_column_type& get_id_column(){ return id_column; }
	
	column_type& get_column(const key_type&);
	
    private:
	id_column_type          id_column;
	std::vector<key_type>	column_ordering;
	__gnu_cxx::hash_map<key_type, column_type*> column_mapping;
};

template <class T>
PlaylistColumns<T>::PlaylistColumns()
    //~ :name("Name"), genre("Genre")
{
    //~ add(c_name);
    this->Gtk::TreeModel::ColumnRecord::add(id_column);
}

template <class T>
void
PlaylistColumns<T>::add(const PlaylistColumns<T>::key_type& name){
    typename PlaylistColumns<T>::column_type& column = *(new typename PlaylistColumns<T>::column_type);
    //~ std::cout << "PlaylistColumns::add() : " << name << "(" << (&column) << ")" << std::endl;
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

//~ class PlaylistListStore : public Gtk::ListStore {
    //~ public:
	//~ PlaylistListStore(){
	    //~ set_column_types(m_Columns);
	//~ }
	
	//~ static void create(){
	    //~ this->Gtk::ListStore::create(m_Columns);
	    //~ return Glib::RefPtr<PlaylistListStore>(this);
	//~ }
	
    //~ private:
	//~ PlaylistColumns m_Columns;
//~ };

//~ class PlaylistView : public Gtk::TreeModel {
    //~ public:
	//~ PlaylistView(){
	    //~ set_column_types(m_Columns);
	//~ }
	
	//~ static void create(){
	    //~ this->Gtk::ListStore::create(m_Columns);
	    //~ return Glib::RefPtr<PlaylistListStore>(this);
	//~ }
	
    //~ private:
	//~ PlaylistColumns m_Columns;
//~ };

class PlaylistView : public Gtk::TreeView {
    public:
	PlaylistView();
	
	void set_model(const Glib::RefPtr<Gtk::TreeModel>&);
	
	//~ static void create(){
	    //~ this->Gtk::ListStore::create(m_Columns);
	    //~ return Glib::RefPtr<PlaylistListStore>(this);
	//~ }
	
	void reset_columns(){}
	
    protected:
	//~ void on_row_activated(const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*);
	bool on_button_press_event(GdkEventButton *);
	bool on_button_release_event(GdkEventButton *);
	
    private:
	ErrorDialog            m_NoStationsErrorDialog;
	StationSelectionDialog m_SelectStationSSDialog;
	//~ PlaylistColumns m_Columns;
};

#endif /* __PLAYLIST_WIDGETS_H__ */
