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

#include <iostream>

#include <vector>
#include <iterator>  // back_inserter
#include <algorithm> // transform

#include "dialog_boxes.h"
#include "playlist.h"

//~ template <typename str>
//~ static std::pair<str, bool> get_item(Gtk::CheckButton *);

/*** class ErrorDialog ***/
ErrorDialog::ErrorDialog(const Glib::ustring &errstr)
	:Gtk::Dialog("Error", true, true),m_errorLabel(errstr),
	 m_centerAlign(Gtk::ALIGN_CENTER),m_okButton("_OK", true)
{
    get_vbox()->set_spacing(3);
    get_vbox()->add(m_errorLabel);
    get_action_area()->set_layout(Gtk::BUTTONBOX_SPREAD);
    get_action_area()->add(m_centerAlign);
    m_centerAlign.add(m_okButton);
    m_okButton.signal_clicked().connect(sigc::bind<int>( sigc::mem_fun(*this, &Gtk::Dialog::response), 0));
    set_border_width(2);
    show_all_children();
}

void ErrorDialog::on_response(int response_id){
    hide();
}

/*** class StationSelectionDialog ***/
StationSelectionDialog::StationSelectionDialog()
	:Gtk::Dialog("Choose Station", true, true),m_selectLabel("Please Select Station"),
	 m_okButton("_Ok", true),m_cancelButton("_Cancel", true)
{
    // make tree view for title selection
    m_Columns.add(m_TitleColumn);
    m_ListStoreRef = Gtk::ListStore::create(m_Columns);
    m_TitleView.set_model(m_ListStoreRef);
    get_vbox()->add(m_selectLabel);
    get_vbox()->add(m_TitleView);
    m_TitleView.append_column(Glib::ustring("Title"), m_TitleColumn);
    
    // add buttons
    get_action_area()->add(m_okButton);
    get_action_area()->add(m_cancelButton);
    
    // connect signals
    m_okButton.signal_clicked().connect( sigc::mem_fun(*this, &StationSelectionDialog::_ok_clicked) );
    m_cancelButton.signal_clicked().connect(sigc::bind<int>( sigc::mem_fun(*this, &Gtk::Dialog::response), -1));
    
    //~ add_button("_OK", 1);
    show_all_children();
}

int StationSelectionDialog::run(const std::vector<Glib::ustring>& titles){
    // put titles in the model
    for(std::vector<Glib::ustring>::const_iterator i = titles.begin(); i != titles.end(); i++){
	dout(8) << "Adding Title: " << *i << std::endl;
	Gtk::TreeModel::iterator iter = m_ListStoreRef->append();
	(*iter)[m_TitleColumn] = *i;
    }
    return Gtk::Dialog::run();
}

void StationSelectionDialog::on_response(int response_id){
    // clear model
    std::cout << "Clear Model" << std::endl;
    m_ListStoreRef->clear();
    
    hide();
}

void StationSelectionDialog::_ok_clicked(){
    dout(7) << "StationSelectionDialog::_get_selection()" << std::endl;
    Gtk::TreeModel::Path selected_path;
    Gtk::TreeViewColumn* focus_col;
    m_TitleView.get_cursor(selected_path, focus_col);
    dout(8) << "Selected Path = " << selected_path.to_string() << std::endl;
    Gtk::Dialog::response(atoi(selected_path.to_string().c_str()));
}

/*** class CheckboxListDialog ***/
CheckboxListDialog::CheckboxListDialog(const CheckboxListDialog::string_type &title, const std::vector<std::pair<CheckboxListDialog::string_type, bool> > &items)
	:Gtk::Dialog(title, false, true)
{
    dout(1) << "CheckboxListDialog::CheckboxListDialog( ... )" << std::endl;
    add_items(items);
    add_button("_OK", 1);
    add_button("_Cancel", 0);
    show_all_children();
}

int CheckboxListDialog::run(){
    return Gtk::Dialog::run();
}

void CheckboxListDialog::add_items(const std::vector<std::pair<string_type, bool> > &items){
    /* Just add items in order given */
    if(items.size()){
	int j = 0;
	for(std::vector<std::pair<string_type, bool> >::const_iterator i = items.begin(); i != items.end(); i++, j++){
	    Gtk::CheckButton *pCB = new Gtk::CheckButton(i->first);
	    pCB->set_active(i->second);
	    get_vbox()->add(*pCB);
	    m_vectorpCheckButton.push_back(pCB);
	    //~ m_vectorItems.push_back(std::make_pair(*i, false));
	    m_vectorItems.push_back(*i);
	    
	    //~ pCB->signal_toggled().connect(sigc::bind<int>( sigc::mem_fun(*this, &CheckboxListDialog::toggled), j));
	}
    }
}

void CheckboxListDialog::on_response(int response_id){
    hide();
    if(response_id){
	/* they pressed OK, so save state */
	m_vectorItems.clear();
	for(std::vector<Gtk::CheckButton*>::iterator i = m_vectorpCheckButton.begin(); i != m_vectorpCheckButton.end(); i++){
	    m_vectorItems.push_back(std::make_pair((*i)->get_label(), (*i)->get_active()));
	}
	//~ std::transform(m_vectorpCheckButton.begin(), m_vectorpCheckButton.end(),
		       //~ std::back_inserter(m_vectorItems), );
    }else{
	/* set widget back to previous values */
	assert(m_vectorpCheckButton.size() == m_vectorItems.size());
	std::vector<std::pair<string_type, bool> >::iterator j = m_vectorItems.begin();
	for(std::vector<Gtk::CheckButton*>::iterator i = m_vectorpCheckButton.begin(); i != m_vectorpCheckButton.end(); i++, j++){
	    (*i)->set_active(j->second);
	}
    }
}

/*** class PreferencesDialog ***/
PreferencesDialog::PreferencesDialog(const Glib::ustring &filename)
	:Gtk::Dialog("Preferences", false, true),m_config(filename),
	 m_ColumnsDialog("Choose Columns to display", m_config.get_columns()),
	 m_numberLabel("Number of entries to get: "),
	 m_URLLabel("URL of XML file: "),
	 m_PlayerLabel("player command: "),
	 m_ColumnsButton("_Columns", true),m_RatingsButton("_Ratings", true)
{
    dout(1) << "PreferencesDialog::PreferencesDialog(" << filename << ")" << std::endl;
    dout(9) << "m_URLEntry.get_width_chars(): " << m_URLEntry.get_width_chars() << std::endl;
    
    m_ColumnsButton.signal_clicked().connect(sigc::hide_return(sigc::mem_fun(m_ColumnsDialog, &CheckboxListDialog::run)));
    //~ m_RatingsButton.signal_clicked().connect(sigc::mem_fun(m_ColumnsDialog, &CheckboxListDialog::run));
    
    /* make gui */
    //~ set_size_request(300, 200);
    m_numberEntry.set_width_chars(4);
    m_leftVBox.pack_start(m_numberLabel, Gtk::PACK_EXPAND_PADDING);
    m_rightVBox.pack_start(m_numberEntry, Gtk::PACK_SHRINK);
    m_URLEntry.set_width_chars(50);
    m_leftVBox.pack_start(m_URLLabel, Gtk::PACK_EXPAND_PADDING);
    m_rightVBox.pack_start(m_URLEntry, Gtk::PACK_SHRINK);
    m_PlayerEntry.set_width_chars(50);
    m_leftVBox.pack_start(m_PlayerLabel, Gtk::PACK_EXPAND_PADDING);
    m_rightVBox.pack_start(m_PlayerEntry, Gtk::PACK_SHRINK);
    
    m_buttonHBox.pack_start(m_ColumnsButton, Gtk::PACK_SHRINK);
    m_buttonHBox.pack_start(m_RatingsButton, Gtk::PACK_SHRINK);
    m_mainHBox.pack_start(m_leftVBox, Gtk::PACK_SHRINK);
    m_mainHBox.pack_start(m_rightVBox, Gtk::PACK_SHRINK);
    get_vbox()->pack_start(m_mainHBox);
    get_vbox()->pack_start(m_buttonHBox);
    add_button("_OK", 1);
    add_button("_Cancel", 0);
    show_all_children();
}

void PreferencesDialog::save() {
    /* save prefs in here */
    m_config.save();
}

int PreferencesDialog::run(){
    /* make sure the real entries are in there */
    m_numberEntry.set_text(m_config.get_num_entries());
    m_URLEntry.set_text(m_config.get_url());
    m_PlayerEntry.set_text(m_config.get_player_cmd());
    //~ m_columns;
    //~ m_ratings;
    return Gtk::Dialog::run();
}

void PreferencesDialog::on_response(int response_id){
    hide();
    if(response_id){
	/* they pressed OK, so save the prefs */
	m_config.set_num_entries(m_numberEntry.get_text());
	m_config.set_url(m_URLEntry.get_text());
	m_config.set_player_cmd(m_PlayerEntry.get_text());
	//~ m_columns;
	//~ m_ratings;
    }
}

Glib::ustring PreferencesDialog::get_url() const {
    return string_subst(m_config.get_url(), m_config.get_num_entries());
}

Glib::ustring PreferencesDialog::get_player_cmd() const {
    return m_config.get_player_cmd();
}

const Configuration::itemlist_type& PreferencesDialog::get_columns() const {
    return m_config.get_columns();
}

const Configuration& PreferencesDialog::get_config() const {
    return m_config;
}

/*** class AboutDialog ***/
AboutDialog::AboutDialog()
	:Gtk::Dialog("About", true, true),
	 m_authorLabel("(C) crass"),
	 m_emailLabel("<crass@users.berlios.de>"),
	 m_descriptionLabel("Show a list of streams from stream database.")
{
    get_vbox()->add(m_authorLabel);
    get_vbox()->add(m_emailLabel);
    get_vbox()->add(m_descriptionLabel);
    add_button("_OK", 1);
    show_all_children();
}

int AboutDialog::run(){
    return Gtk::Dialog::run();
}

void AboutDialog::on_response(int response_id){
    hide();
}
