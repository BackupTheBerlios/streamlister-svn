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

#include <iostream>

#include <ext/hash_map>

#include <libxml++/libxml++.h>

//~ using namespace std;
//~ using namespace xmlpp;

/*
 * Classes for holding tvlisting data
*/
class PlaylistEntry{
    public:
	typedef Glib::ustring string_type;
    
    public:
	PlaylistEntry(const xmlpp::Element *);
    
	std::ostream& print(std::ostream&) const;
    
	// accessor functions
	string_type get_data(const string_type&) const;
	
	string_type get_name() const { return name; }
	string_type get_stationid() const { return stationid; }
	string_type get_genre() const { return genre; }
	string_type get_nowplaying() const { return nowplaying; }
	string_type get_listeners() const { return listeners; }
	string_type get_bitrate() const { return bitrate; }
	string_type get_rating() const { return rating; }
	string_type get_playstring() const { return playstring; }
    
    private:
	__gnu_cxx::hash_map<string_type, string_type> data_map;
	string_type name;
	string_type stationid;
	string_type genre;
	string_type nowplaying;
	string_type listeners;
	string_type bitrate;
	string_type rating;
	string_type playstring;
    
};

std::ostream& operator << (std::ostream &, const PlaylistEntry& );

class Playlist{
    public:
	typedef Glib::ustring string_type;
    
    public:
	Playlist(const string_type & = string_type());
	
	std::vector<PlaylistEntry>& get_entries() { return m_entries; }
	const std::vector<PlaylistEntry>& get_entries() const { return m_entries; }
	
	void clear();
	
	void parse_memory(const string_type & = string_type());
	void parse_file(const string_type & = string_type());
	//~ bool saveto_file(const string_type & = string_type());
	bool saveto_file(const string_type &);
	
	const std::vector<string_type>& get_genres() const{return m_genre_list;}
	const std::vector<string_type>& get_ratings() const{return m_ratings;}
	
	const std::vector<string_type>& get_properties() const {
	    return m_properties;
	}
    
    private:
	void _parse_document();
    
    private:
	std::vector<string_type>   m_properties; // FIXME: hack!!!
	
	int                          m_numEntries;
	string_type                  m_label;
	string_type                  m_filename;
	std::vector<PlaylistEntry>   m_entries;
	std::vector<string_type>     m_genre_list;
	std::vector<string_type>     m_ratings;
	
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
	typedef Glib::ustring string_type;
    
    public:
	PlaylistFile(const string_type &);
	
	// FIXME: this could be better
	void parse_file(const string_type &);
	
	const std::vector<PlaylistFileDesc>& get_files() const {
	    return files;
	}
	
	std::vector<string_type> get_titles() const;
	
	size_type size() const { return files.size(); }
	
    private:
	void _parse_equals(const string_type &, __gnu_cxx::hash_map<string_type, string_type>&);
    
    private:
	int numberofentries;
	int version;
	std::vector<PlaylistFileDesc> files;
};

#endif /* __PLAYLIST_H__ */
