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

#include "dialog_boxes.h"

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
	std::cout << "Adding Title: " << *i << std::endl;
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
    std::cout << "StationSelectionDialog::_get_selection()" << std::endl;
    Gtk::TreeModel::Path selected_path;
    Gtk::TreeViewColumn* focus_col;
    m_TitleView.get_cursor(selected_path, focus_col);
    std::cout << "Selected Path = " << selected_path.to_string() << std::endl;
    Gtk::Dialog::response(atoi(selected_path.to_string().c_str()));
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

void AboutDialog::run(){
    Gtk::Dialog::run();
}

void AboutDialog::on_response(int response_id){
    hide();
}
