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

#include <cstdlib>
#include <cassert>

#include <iostream>

#include <gtkmm.h>

#include "playlist.h"

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

class AboutDialog : public Gtk::Dialog {
    public:
	AboutDialog();
    
	void run();
    
    protected:
	virtual void on_response(int);
    
    private:
	Gtk::Label m_authorLabel;
	Gtk::Label m_emailLabel;
	Gtk::Label m_descriptionLabel;
};

#endif /* __DIALOG_BOXES_H__ */
