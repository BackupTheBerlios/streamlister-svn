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

#include "player_window.h"
#include "utility.h"

#include <sys/types.h>	// 
#include <sys/wait.h>	// waitpid
#include <signal.h>	// kill
#include <unistd.h>	// system, close
#include <fcntl.h>	// open, fcntl
#include <string.h>	// strerror_r
#include <ctype.h>	// is*()

#include <cerrno>
#include <cassert>

#include <gtkmm/socket.h>
#include <gtkmm/plug.h>
#include <gdkmm/event.h>
#include <gdk/gdk.h>

#define _WAIT_IN_CTOR

/* Given a string like "mplayer  -v -nocache -wid 291504338 'http://70.84.33.194:8150;stream.nsv'"
 * split by spaces, return arg array of const char*, and prog = 'mplayer'
 */
std::vector<char*>
make_exec_args(const PlayerWindow::string_type &cmd){
    std::vector<std::string> argv;
    bool inquote = false;
    int pos = cmd.find(' ');
    dout(3) << "cmd = " << cmd << std::endl;
    
    argv.push_back(cmd.substr(0, pos));
    dout(3) << "Program name = " << argv[0] << std::endl;
    pos++;
    
    argv.push_back(std::string());
    for(; pos < cmd.length(); pos++){
	char c=cmd[pos];
	//~ dout(3) << c << std::endl;
	if(!isascii(c) || !isprint(c)){
	    dout(3) << "Bad character in command string (" << c << ")" << std::endl;
	    return std::vector<char*>();
	}
	if(isspace(c) && !inquote){
	    if((*(--argv.end())).length()){
		//~ dout(3) << "arg: '" << (*(--argv.end())) << "'" << std::endl;
		argv.push_back(std::string());
	    }else
		dout(3) << "Multiple spaces found" << std::endl;
	//~ }else if(c == '"'){
	}else if(c == '\''){
	    //~ if(inquote){
	    //~ }
	    inquote = !inquote;
	}else{
	    //~ dout(3) << "adding: " << c << "(" << pos << ")" << std::endl;
	    (*(--argv.end())) += c;
	}
    }
    
    std::vector<char*> ret(argv.size());
    std::vector<char*>::iterator j = ret.begin();
    for(std::vector<std::string>::iterator i=argv.begin(); i != argv.end(); i++, j++){
	//~ dout(3) << "arg: '" << (*i) << "'" << std::endl;
	char *x = new char[i->length()+1];
	*j = strncpy(x, i->c_str(), i->length()+1);
	//~ dout(3) << "*j: '" << (*j) << "'" << std::endl;
	//~ strncpy(x, i->c_str(), i->length()+1);
	//~ *j = std::auto_ptr<char>(x);
    }
    
    dout(3) << "END make_exec_args (" << pos << ")" << std::endl;
    return ret;
}

PlayerWindow::PlayerWindow(string_type cmd, string_type streamurl):
	m_player_pid(0), m_chld_stdin(-1), m_chld_stderr(-1){
    Gdk::NativeWindow wid;
    Gtk::Socket mp_socket;
    bool use_window = cmd.find("%w") < string_type::npos;
    Glib::ustring player_cmd = string_subst(cmd, streamurl);
    int newstderr, newstdin;
    
    if(use_window){
	string_type wid_str;
	this->add(mp_socket);
	
	// The following call is only necessary if one of the ancestors of the
	// socket is not yet visible
	//~ socket.realize();
	
	dout(3) << "The ID of the sockets window is: " << mp_socket.get_id() << std::endl;
	
	// single int -> string routine
	for(wid = mp_socket.get_id(); wid; wid /= 10){
	    //~ wid_str += ('0' + (wid % 10));
	    wid_str.insert(0, 1, (char)('0' + (wid % 10)));
	}
	
	dout(3) << "wid_str = " << wid_str << std::endl;
	m_swid = mp_socket.get_id();
	player_cmd = string_subst(player_cmd, wid_str, string_type("%w"));
	
	set_keep_above(); // always on top
	resize(320, 240);
	//~ move(200, 200);
	show_all_children();
	show();
	present();
    }
    
    //~ mp_socket.signal_plug_removed().connect(sigc::mem_fun(*this, &PlayerWindow::on_delete_event));
    mp_socket.signal_plug_removed().connect(sigc::mem_fun(*this, &PlayerWindow::on_plug_removed));
    
    //~ dout << "mplayer -v -nocache '" << streamurl << "'" << std::endl;
    dout(3) << player_cmd << std::endl;
    
    /* set close-on-exec flag */
    fcntl(STDOUT_FILENO, F_SETFD, 1);
    fcntl(STDIN_FILENO, F_SETFD, 1);
    
    /* setup to grab stderr */
    {
	int pipe_ends[2] = {0, 0}, newstderr;
	if(pipe(pipe_ends) == -1){
	    dout(3) << "Error: " << strerror(errno) << std::endl;
	}
	m_chld_stderr = pipe_ends[0];
	newstderr = pipe_ends[1];
    }
    
    /* setup to grab stdin */
    {
	int pipe_ends[2] = {0, 0}, newstdin;
	if(pipe(pipe_ends) == -1){
	    dout(3) << "Error: " << strerror(errno) << std::endl;
	}
	m_chld_stdin = pipe_ends[1];
	newstdin = pipe_ends[0];
    }
    
    /* NOTE: it might be nice to do some command piping to MPlayer via
     * -input command, but this wouldn't be general for all players */
    
    //~ char *args[] = {};
    m_player_pid = fork();
    if(m_player_pid == 0){
	/* in child, do an exec */
	/* put us in our new session, 
	 * this should get rid of zombies */
	if(setsid() == -1){
	    perror("Error: ");
	}
	//~ setpgid(pid,0);
	
#if 1 /* for execvp */
	//~ Gtk::Plug mp_plug(m_swid);
	Gtk::Plug mp_plug(m_swid);
	dout(3) << "The ID of the plugs window is: " << mp_plug.get_id() << std::endl;
	
	//~ Gtk::Widget mp_window;
	//~ mp_window.set_parent_window();
	//~ gdk_window_foreign_new(GdkNativeWindow anid)
	
	//~ Gtk::Socket mp_socket_int;
	//~ mp_plug.add(mp_socket_int);
	//~ dout(3) << "The ID of the internal socket window is: " << mp_socket_int.get_id() << std::endl;
	
	// single int -> string routine
	string_type wid_str;
	//~ for(wid = mp_socket_int.get_id(); wid; wid /= 10)
	//~ for(wid = mp_plug.get_id(); wid; wid /= 10)
	    //~ wid_str.insert(0, 1, (char)('0' + (wid % 10)));
	//~ player_cmd = string_subst(player_cmd, wid_str, string_type("%w"));
	
	//~ std::auto_ptr<char> ap = make_exec_args(player_cmd);
	std::vector<char*> argv_ap = make_exec_args(player_cmd);
	
	char **argv = new char*[argv_ap.size()+1], **a = argv;
	for(std::vector<char*>::iterator i=argv_ap.begin(); i != argv_ap.end(); i++){
	    *a = *i;
	    //~ dout(3) << "*a = " << *a << std::endl;
	    a++;
	}
	*a = NULL;
#endif
	
	/* FIXME:  do an execve (fuck it do it later) */
#if 1
	/* Wtf, the app kills the shells stdout, but how can this do that?
	 * After a fork() the closing of copied fd's don't close the fd in the other process */
	//~ ::close(STDOUT_FILENO);
	/* make the output descriptor point to null so it doesn't interfere with streamlister output */
	//~ if((newstdout = open("/dev/null", O_WRONLY)) != STDOUT_FILENO){
	    //~ dout(1) << "Error: open(\"/dev/null\", O_WRONLY) failed to open as stdout [" << STDOUT_FILENO << "]" << std::endl;
	    //~ exit(0);
	//~ }
	
	int newstdout = 0;
	//~ if((newstdout = open("/dev/null", O_WRONLY)) == -1){
	if((newstdout = open("/tmp/mplayer.out", O_WRONLY|O_CREAT)) == -1){
	    dout(1) << "Error: open(\"/dev/null\", O_WRONLY) failed to open as stdout [" << STDOUT_FILENO << "]" << std::endl;
	    exit(0);
	}
	int oldstdout = dup(STDOUT_FILENO);
	dup2(newstdout, STDOUT_FILENO);
	
	/* grab stderr, stdin */
	dup2(newstderr, STDERR_FILENO);
	dup2(newstdin, STDIN_FILENO);
#endif
	
	
	//~ daemon(0, 1 /*0*/);
	
	//~ if(system(player_cmd.c_str()) == -1)
	    //~ dout(1) << "system() errored" << std::endl;
	//~ exit(0);
	
	//~ if(ap.get())
	    //~ execvp(((char**)ap.get())[0], (char **)ap.get());
	execvp(argv[0], argv);
	dout(1) << "NEVER REACH HERE!!" << std::endl;
	//~ sleep(100);
	
	for(a = argv; *a; a++)
	    delete [] a;
	
	/* should never get here, but its good programming */
	delete [] argv;
    }else{
	dout(3) << "m_player_pid = " << m_player_pid << std::endl;
	
#ifdef _WAIT_IN_CTOR
	int status=0, ret=0, i=0;
	while((ret = waitpid(m_player_pid, &status, WNOHANG)) == 0 || (ret != m_player_pid && ret != -1)){
	    /* if there are gtk event pending then process them, this allows the main */
	    /* window to be usable while waiting */
	    if(Gtk::Main::events_pending()){
		//~ dout(3) << i++ << ": processing events" << std::endl;
		Gtk::Main::iteration();
	    }
	}
	if(ret == -1){
	    dout(3) << "Error: " << strerror(errno) << std::endl;
	}else{
	    assert(ret == m_player_pid);
	    //~ assert(WIFSIGNALED(status));
	}
	dout(3) << m_player_pid << ": waitpid returned" << std::endl;
	m_player_pid = 0;
	dout(3) << "deleting this in ctor" << std::endl;
	//~ delete this;
#endif
    }
    
    //~ delete this;
    //~ PlayerWindow::~PlayerWindow();
}

void PlayerWindow::close(){
    //~ this->Gtk::Window::close();
    delete this;
}

bool PlayerWindow::on_delete_event(GdkEventAny *event){
//~ bool PlayerWindow::on_delete_event(){
    int status=0, ret=0;
    dout(3) << "B:on_delete_event (" << m_player_pid << ")" << std::endl;
    if(event)
	dout(3) << event->type << ", " << event->window << ", " << event->send_event << std::endl;
    //~ this->Gtk::Window::close();
    dout(3) << "E:on_delete_event (" << m_player_pid << ")" << std::endl;
    
    if(!m_player_pid){
	dout(3) << "m_player_pid = 0, no killing" << std::endl;
    //~ }else if(kill(m_player_pid, SIGTERM) == -1){
    }else if(ret = waitpid(m_player_pid, &status, WNOHANG)){
	dout(3) << m_player_pid << ": process died of natural causes" << std::endl;
    }else if(kill(m_player_pid, SIGKILL) == -1){
	dout(3) << strerror(errno) << std::endl;
    }else{
	//~ int status=0, ret=0;
	dout(3) << "Killed player " << m_player_pid << std::endl;
#ifndef _WAIT_IN_CTOR
	while((ret = waitpid(m_player_pid, &status, WNOHANG)) == 0){
	    /* if there are gtk event pending then process them, this allows the main */
	    /* window to be usable while waiting */
	    if(Gtk::Main::events_pending(true)){
	    //~ if(Gtk::Main::events_pending()){
		Gtk::Main::iteration();
	    }
	}
	if(ret == -1){
	    dout(3) << strerror(errno) << std::endl;
	}else{
	    assert(ret == m_player_pid);
	    assert(WIFSIGNALED(status));
	}
	dout(3) << "waitpid returned" << std::endl;
	m_player_pid = 0;
#endif
	//~ return false;
    }
    //~ sleep(20);
    
#ifndef _WAIT_IN_CTOR
    delete this;
#endif
    return false;
}

bool PlayerWindow::on_plug_removed(){
    dout(3) << "on_plug_removed (" << m_player_pid << ")" << std::endl;
    assert(0); /* is this ever called? */
    return on_delete_event();
}

bool PlayerWindow::on_key_press_event(GdkEventKey* event){
    assert(event);
    dout(3) << "on_key_press_event ( {" << event->keyval << ","
            << event->hardware_keycode << "," << event->time << ","
            << event->state << " })" << std::endl;
    /* send this key press to the child process */
    //~ Glib::ustring s((char)event->keyval);
    std::string s = std::string() + (char)event->keyval;
    
    int wlen = 0;
    dout(3) << s.c_str() << " (" << s.length() << ")" << std::endl;
    if((wlen = write(m_chld_stdin, s.c_str(), 1)) == -1){
	dout(3) << strerror(errno) << std::endl;
    }
    dout(3) << "wlen = " << wlen << std::endl;
    
    /* send through to the underlying window */
    //~ Gdk::Event((GdkEvent*)event).send_client_message(m_swid);
    Gdk::Event((GdkEvent*)event).put();
    
    
    return true;
}
