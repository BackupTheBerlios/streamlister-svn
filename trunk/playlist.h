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
#ifndef __PLAYLIST_H__
#define __PLAYLIST_H__

#define CURL_SYSTEM "curl -0 -A 'User-Agent:Winamp/5.0' 'http://www.shoutcast.com/sbin/tvlister.phtml?limit=500&service=winamp2' -o nsv_tvlisting"
//~ #define SHOUTCAST_URL "http://www.shoutcast.com/sbin/tvlister.phtml?limit=500&service=winamp2"
#define SHOUTCAST_URL "http://www.shoutcast.com/sbin/tvlister.phtml?limit=%s&service=winamp2&no_compress=1"
#define PLAYER_CMD "mplayer -v -nocache '%s'"
#define DEFAULT_NUMBER_ENTRIES "500"
#define DEFAULT_CONFIG_PATH "~/.streamlisterrc"

#include <cstdlib>
#include <cassert>
#include <stdexcept>

#include <iostream>

#include <string>
#include <ext/hash_set>
#include <ext/hash_map>

#include <libxml++/libxml++.h>

#include "utility.h"

//~ using namespace std;
//~ using namespace xmlpp;

/*
 * Classes for holding tvlisting data
*/
class PlaylistEntry{
    public:
	PlaylistEntry(const xmlpp::Element *);
    
	std::ostream& print(std::ostream&) const;
    
	// accessor functions
	std::string get_data(const std::string&) const;
	
	std::string get_name() const { return name; }
	std::string get_stationid() const { return stationid; }
	std::string get_genre() const { return genre; }
	std::string get_nowplaying() const { return nowplaying; }
	std::string get_listeners() const { return listeners; }
	std::string get_bitrate() const { return bitrate; }
	std::string get_rating() const { return rating; }
	std::string get_playstring() const { return playstring; }
    
    private:
	__gnu_cxx::hash_map<Glib::ustring, Glib::ustring> data_map;
	std::string name;
	std::string stationid;
	std::string genre;
	std::string nowplaying;
	std::string listeners;
	std::string bitrate;
	std::string rating;
	std::string playstring;
    
};

std::ostream& operator << (std::ostream &, const PlaylistEntry& );

class Playlist{
    public:
	Playlist(const std::string & = std::string());
	
	std::vector<PlaylistEntry>& get_entries() { return m_entries; }
	const std::vector<PlaylistEntry>& get_entries() const { return m_entries; }
	
	void clear();
	
	void parse_file(const std::string & = std::string());
	//~ bool saveto_file(const std::string & = std::string());
	bool saveto_file(const std::string &);
	
	const std::vector<Glib::ustring>& get_genres() const{
	    return m_genre_list;
	}
	
	const std::vector<Glib::ustring>& get_properties() const {
	    return m_properties;
	}
    
    private:
	std::vector<Glib::ustring>   m_properties; // FIXME: hack!!!
	
	int                          m_numEntries;
	std::string                  m_label;
	std::string                  m_filename;
	std::vector<PlaylistEntry>   m_entries;
	std::vector<Glib::ustring>   m_genre_list;
	std::vector<Glib::ustring>   m_ratings;
	
	//~ xmlpp::Document              m_xmldocument;
	xmlpp::DomParser             m_xmldocument;
};


class PlaylistFile {
    public:
	struct PlaylistFileDesc{
	    Glib::ustring filename;
	    Glib::ustring title;
	    int length;
	};
    
    public:
	typedef std::vector<PlaylistFileDesc>::size_type size_type;
    
    public:
	PlaylistFile(const Glib::ustring &);
	
	// FIXME: this could be better
	void parse_file(const Glib::ustring &);
	
	const std::vector<PlaylistFileDesc>& get_files() const {
	    return files;
	}
	
	std::vector<Glib::ustring> get_titles() const;
	
	size_type size() const { return files.size(); }
	
    private:
	void _parse_equals(const Glib::ustring &, __gnu_cxx::hash_map<Glib::ustring, Glib::ustring>&);
    
    private:
	int numberofentries;
	int version;
	std::vector<PlaylistFileDesc> files;
};

#endif /* __PLAYLIST_H__ */
