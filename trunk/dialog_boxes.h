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

#ifndef __DIALOG_BOXES_H__
#define __DIALOG_BOXES_H__

#include <vector>
#include <utility> // pair

#include <gtkmm.h>

#include "utility.h"
#include "configuration.h"

class ErrorDialog : public Gtk::Dialog {
    public:
	ErrorDialog(const Glib::ustring &errstr);
    
    protected:
	virtual void on_response(int);
    
    private:
	Gtk::Label m_errorLabel;
	Gtk::Alignment m_centerAlign;
	Gtk::Button m_okButton;
};

class StationSelectionDialog : public Gtk::Dialog {
    public:
	StationSelectionDialog();
    
	int run(const std::vector<Glib::ustring>&);
    
    protected:
	virtual void on_response(int);
    
    private:
	void _ok_clicked();
    
    private:
	Gtk::Label m_selectLabel;
    
	Gtk::Button m_okButton;
	Gtk::Button m_cancelButton;
    
	// List widgets:
	Gtk::TreeModelColumn<Glib::ustring> m_TitleColumn;
	Gtk::TreeModel::ColumnRecord m_Columns;
	Glib::RefPtr<Gtk::ListStore> m_ListStoreRef;
	Gtk::TreeView m_TitleView;
	
};

//~ template <typename T = Glib::ustring>
class CheckboxListDialog : public Gtk::Dialog{
    public:
	typedef Glib::ustring string_type;
	typedef Configuration::item_type item_type;
	typedef Configuration::itemlist_type itemlist_type;
    
    public:
	//~ CheckboxListDialog(const string_type&, const std::vector<std::pair<string_type, bool> > & = (std::vector<std::pair<string_type, bool> >()));
	CheckboxListDialog(const string_type&, itemlist_type &);
	
	int run();
	
	//~ void set_items(const std::vector<string_type> &);
	const itemlist_type & get_items() const{return m_vectorItems;}
    
    protected:
	virtual void add_items(const itemlist_type &);
	virtual void on_response(int response_id);
	//~ virtual void toggled(int);
    
    private:
	std::vector<Gtk::HBox> m_vectorHBox;
	std::vector<Gtk::CheckButton*> m_vectorpCheckButton;
	itemlist_type &m_vectorItems;
};

//~ class RatingsDialog : public CheckboxListDialog{
    //~ public:
	//~ typedef CheckboxListDialog::string_type string_type;
	//~ typedef CheckboxListDialog::item_type item_type;
	//~ typedef CheckboxListDialog::itemlist_type itemlist_type;
    
    //~ public:
	//~ RatingsDialog(const string_type&, itemlist_type &);
    
//~ };

class PreferencesDialog : public Gtk::Dialog{
    public:
	typedef Glib::ustring string_type;
    
    public:
	PreferencesDialog(const string_type &filename=string_type());
	
	int run();
	
	string_type get_url() const;
	string_type get_player_cmd() const;
	const Configuration::itemlist_type& get_columns();
	const Configuration& get_config() const;
	void save();
    
    protected:
	virtual void on_response(int);
    
    private:
	/* preferences data */
	Configuration m_config;
    
    private:
	Gtk::HBox  m_mainHBox;
	Gtk::VBox  m_leftVBox;
	Gtk::VBox  m_rightVBox;
	
	Gtk::HBox  m_numberHBox;
	Gtk::Label m_numberLabel;
	Gtk::Entry m_numberEntry;
	Gtk::HBox  m_URLHBox;
	Gtk::Label m_URLLabel;
	Gtk::Entry m_URLEntry;
	Gtk::HBox  m_PlayerHBox;
	Gtk::Label m_PlayerLabel;
	Gtk::Entry m_PlayerEntry;
	
	Gtk::HBox   m_buttonHBox;
	Gtk::Button m_ColumnsButton;
	Gtk::Button m_RatingsButton;
	
	CheckboxListDialog m_ColumnsDialog;
	CheckboxListDialog m_RatingsDialog;
};

class AboutDialog : public Gtk::Dialog {
    public:
	AboutDialog();
	int run();
    
    protected:
	virtual void on_response(int);
    
    private:
	Gtk::Label m_authorLabel;
	Gtk::Label m_emailLabel;
	Gtk::Label m_descriptionLabel;
};

#endif /* __DIALOG_BOXES_H__ */
