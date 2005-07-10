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

#include <sys/types.h>

#include <gtkmm.h>

#ifndef __PLAYER_WINDOW_H__
#define __PLAYER_WINDOW_H__

class PlayerWindow : public Gtk::Window {
    public:
	typedef Glib::ustring string_type;
    
    public:
	PlayerWindow(string_type cmd, string_type url);
	void close();
    
    protected:
	virtual bool on_delete_event(GdkEventAny *event=NULL);
	virtual bool on_plug_removed();
	virtual bool on_key_press_event(GdkEventKey* event);
    
    private:
	//~ void _
    
    private:
	pid_t m_player_pid;
	int m_chld_stdin, m_chld_stdout, m_chld_stderr;
	Gdk::NativeWindow m_swid;
};

#endif /* __PLAYER_WINDOW_H__ */
