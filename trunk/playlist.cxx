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
#include <stdexcept>

#include <iostream>

#include "utility.h"

#include <ext/hash_set>
#include <ext/hash_map>

#include "playlist.h"
//~ #include "utility.h"

/*** class PlaylistEntry ***/

PlaylistEntry::PlaylistEntry(const xmlpp::Element *entryEl){
    xmlpp::Attribute *anode = entryEl->get_attribute("Playstring");
    if (anode != NULL){
	playstring = anode->get_value();
    }
    
    xmlpp::Node::NodeList children = entryEl->get_children();
    for(xmlpp::Node::NodeList::iterator j = children.begin(); j != children.end(); j++){
	xmlpp::TextNode *tnode = (static_cast<xmlpp::Element *>(*j))->get_child_text();
	
	if (tnode == NULL)
	    continue;
	else if (std::string("Name") == (static_cast<xmlpp::Element *>(*j))->get_name()){
	    name = tnode->get_content();
	}else if(std::string("StationID") == (static_cast<xmlpp::Element *>(*j))->get_name()){
	    stationid = tnode->get_content();
	}else if(std::string("Genre") == (static_cast<xmlpp::Element *>(*j))->get_name()){
	    genre = tnode->get_content();
	}else if(std::string("Nowplaying") == (static_cast<xmlpp::Element *>(*j))->get_name()){
	    nowplaying = tnode->get_content();
	}else if(std::string("Listeners") == (static_cast<xmlpp::Element *>(*j))->get_name()){
	    listeners = tnode->get_content();
	}else if(std::string("Bitrate") == (static_cast<xmlpp::Element *>(*j))->get_name()){
	    bitrate = tnode->get_content();
	}else if(std::string("Rating") == (static_cast<xmlpp::Element *>(*j))->get_name()){
	    rating = tnode->get_content();
	}
    }
}

std::ostream& PlaylistEntry::print(std::ostream& out) const{
    out << "Name: " << name << std::endl;
    //~ out << "  Station ID: " << stationid << std::endl;
    out << "  Genre: " << genre << std::endl;
    out << "  Now Playing: " << nowplaying << std::endl;
    //~ out << "  Listeners: " << listeners << std::endl;
    //~ out << "  Bitrate: " << bitrate << std::endl;
    out << "  Rating: " << rating << std::endl;
    //~ out << "  Playstring: " << playstring << std::endl;
    return out;
}

// accessor functions
PlaylistEntry::string_type PlaylistEntry::get_data(const PlaylistEntry::string_type& which) const {
    PlaylistEntry::string_type retstr("");
    if (PlaylistEntry::string_type("Name") == which){
	retstr = name;
    }else if(PlaylistEntry::string_type("StationID") == which){
	retstr = stationid;
    }else if(PlaylistEntry::string_type("Genre") == which){
	retstr = genre;
    }else if(PlaylistEntry::string_type("Nowplaying") == which){
	retstr = nowplaying;
    }else if(PlaylistEntry::string_type("Listeners") == which){
	retstr = listeners;
    }else if(PlaylistEntry::string_type("Bitrate") == which){
	retstr = bitrate;
    }else if(PlaylistEntry::string_type("Rating") == which){
	retstr = rating;
    }else if(PlaylistEntry::string_type("Playstring") == which){
	retstr = playstring;
    }
    return retstr;
}

//~ std::string PlaylistEntry::get_name() const { return name; }
//~ std::string PlaylistEntry::get_stationid() const { return stationid; }
//~ std::string PlaylistEntry::get_genre() const { return genre; }
//~ std::string PlaylistEntry::get_nowplaying() const { return nowplaying; }
//~ std::string PlaylistEntry::get_listeners() const { return listeners; }
//~ std::string PlaylistEntry::get_bitrate() const { return bitrate; }
//~ std::string PlaylistEntry::get_rating() const { return rating; }
//~ std::string PlaylistEntry::get_playstring() const { return playstring; }

std::ostream& operator << (std::ostream &out, const PlaylistEntry& pe){
    //~ out << "Trying to output a PlaylistEntry" << endl;
    return pe.print(out);
}

/*** class Playlist ***/

Playlist::Playlist(const Playlist::string_type &file){
    dout(7) << "Playlist::Playlist()" << std::endl;
    m_filename = file;
    
    /* FIXME: These are the statically defined tags, but for this to be more general this
     * should be dynamic based on tags as we see them */
    m_properties.push_back("Name");
    m_properties.push_back("StationID");
    m_properties.push_back("Genre");
    m_properties.push_back("Nowplaying");
    m_properties.push_back("Listeners");
    m_properties.push_back("Bitrate");
    m_properties.push_back("Rating");
    m_properties.push_back("Playstring");
}

//~ std::vector<PlaylistEntry>& Playlist::get_entries() { return entries; }
//~ const std::vector<PlaylistEntry>& Playlist::get_entries() const { return entries; }

void Playlist::clear() { m_numEntries = 0; m_label = ""; 
    m_entries.clear(); m_genre_list.clear(); }

void Playlist::parse_memory(const Playlist::string_type &data){
    dout(7) << "Parsing memory" << std::endl;
    m_xmldocument.parse_memory(data);
    _parse_document();
}

void Playlist::parse_file(const Playlist::string_type & file){
    
    Playlist::string_type filename(file);
    if (filename == ""){
	if (m_filename != "")
	    filename = m_filename;
	else
	    throw std::runtime_error("No file specified");
    }
    
    dout(7) << "Parsing file: " << filename << std::endl;
    
    m_xmldocument.parse_file(filename);
    _parse_document();
}
    
void Playlist::_parse_document(){
    // empty the old playlist
    clear();
    
    xmlpp::Document *doc = m_xmldocument.get_document();
    
    xmlpp::Element *cur_element = doc->get_root_node();
    
    dout(9) << "path: " << cur_element->get_path() << std::endl;
    
    xmlpp::Node::NodeList playlists = cur_element->get_children(std::string("playlist"));
    
    if (playlists.size() != 1){
	std::cerr << "0 or more than 1 playlists: " << playlists.size() << std::endl;
	throw std::runtime_error("Incorrect Number of playlists");
    }
    
    // points to playlist element
    cur_element = static_cast<xmlpp::Element *>(*(playlists.begin()));
    
    xmlpp::Element::AttributeList attrlist = cur_element->get_attributes();
    
    xmlpp::Element::AttributeList::iterator i = attrlist.begin();
    for(;i != attrlist.end(); i++){
	dout(9) << "Name: " << (*i)->get_name() << ", Value: " << (*i)->get_value() << std::endl;
    }
    
    xmlpp::NodeSet nset = cur_element->find(std::string("entry"));
    
    __gnu_cxx::hash_set<std::string, __gnu_cxx::hash<std::string> > genre_hashset, ratings_hashset;
    
    // loop over all entry nodes, putting them into entry list, and making hash_set of genres
    for(xmlpp::NodeSet::iterator j = nset.begin(); j != nset.end(); j++){
	m_entries.push_back(PlaylistEntry((static_cast<xmlpp::Element *>(*j))));
	//~ std::cout << entries.rbegin()->get_genre() << std::endl;
	genre_hashset.insert(m_entries.rbegin()->get_data("Genre"));
	ratings_hashset.insert(m_entries.rbegin()->get_data("Rating"));
	//~ std::cout << *(entries.rbegin());
    }
    
    // now put into vector sorted
    //~ std::vector<std::string> genre_list_unsorted;
    //~ for(__gnu_cxx::hash_set<std::string>::iterator i = genre_hashset.begin();
	//~ i != genre_hashset.end(); genre_list_unsorted.push_back(*i), i++);
    m_genre_list.resize(genre_hashset.size());
    std::partial_sort_copy(genre_hashset.begin(), genre_hashset.end(),
			   m_genre_list.begin(), m_genre_list.end());
    
    /* also sort ratings */
    m_ratings.resize(ratings_hashset.size());
    std::partial_sort_copy(ratings_hashset.begin(), ratings_hashset.end(),
			   m_ratings.begin(), m_ratings.end());
    
    dout(9) << "m_ratings.size() = " << m_ratings.size() << std::endl;
    for(std::vector<Glib::ustring>::iterator i = m_ratings.begin(); i != m_ratings.end(); i++){
	dout(9) << *i << std::endl;
    }
}

bool Playlist::saveto_file(const Playlist::string_type &filename){
    //~ m_xmldocument.get_document()->write_to_file(filename);
    m_xmldocument.get_document()->write_to_file_formatted(filename);
    return true;
}

//~ const std::vector<std::string>& Playlist::get_genres() const{
    //~ return genre_list;
//~ }

//~ const std::vector<Glib::ustring>& Playlist::get_properties() const {
    //~ return properties;
//~ }

/*** class PlaylistFile ***/

PlaylistFile::PlaylistFile(const PlaylistFile::string_type &plsfile):numberofentries(0),version(0){
    parse_file(plsfile);
}

// FIXME: this could be better
void PlaylistFile::parse_file(const PlaylistFile::string_type &plsfile){
    // parse the pls file
    if (plsfile.compare(0, 10, "[playlist]") == 0){
	dout(8) << "found playlist header" << std::endl;
	
	__gnu_cxx::hash_map<PlaylistFile::string_type, PlaylistFile::string_type> map;
	
	_parse_equals(plsfile.substr(10), map);
	
	__gnu_cxx::hash_map<PlaylistFile::string_type, PlaylistFile::string_type>::iterator iter = map.find("numberofentries");
	if(iter != map.end()){
	    numberofentries = atoi((iter->second).c_str());
	    
	    PlaylistFileDesc pls_desc;
	    PlaylistFile::string_type usFile("File_"), usTitle("Title_"), usLength("Length_");
	    for(int i=0; i < numberofentries; i++){
		// index starts at 1
		//~ usFile.assign(4, (char)('1' + i));
		//~ usTitle.assign(5, (char)('1' + i));
		//~ usLength.assign(6, (char)('1' + i));
		usFile.erase(4, 1).push_back((char)('1' + i));
		usTitle.erase(5, 1).push_back((char)('1' + i));
		usLength.erase(6, 1).push_back((char)('1' + i));
		
		iter = map.find(usFile);
		if(iter != map.end()){
		    pls_desc.filename = (*iter).second;
		}else
		    std::cerr << "+++ERROR: could not find key: " << usFile << std::endl;
		
		iter = map.find(usTitle);
		if(iter != map.end()){
		    pls_desc.title = (*iter).second;
		}else
		    std::cerr << "+++ERROR: could not find key: " << usTitle << std::endl;
		
		iter = map.find(usLength);
		if(iter != map.end()){
		    pls_desc.length = atoi((iter->second).c_str());
		}else
		    std::cerr << "+++ERROR: could not find key: " << usLength << std::endl;
		
		files.push_back(pls_desc);
	    }
	    
	    if((iter = map.find("Version")) != map.end()){
		version = atoi((iter->second).c_str());
	    }else
		std::cerr << "+++ERROR: could not find 'Version'" << std::endl;
	    
	}else
	    std::cerr << "+++ERROR: could not find 'numberofentries'" << std::endl;
	
    }else{
	// throw exception
	
	std::cerr << "+++ERROR: could not find playlist header" << std::endl;
	dout(1) << "########################################" << std::endl;
	dout(1) << plsfile << std::endl;
	dout(1) << "########################################" << std::endl;
    }
    
}

//~ const std::vector<PlaylistFileDesc>& PlaylistFile::get_files() const {
    //~ return files;
//~ }

std::vector<PlaylistFile::string_type> PlaylistFile::get_titles() const{
    std::vector<PlaylistFile::string_type> titles;
    for(std::vector<PlaylistFileDesc>::const_iterator i = files.begin(); i != files.end(); i++){
	titles.push_back(i->title);
    }
    return titles;
}

//~ size_type PlaylistFile::size() const { return files.size(); }

void PlaylistFile::_parse_equals(const PlaylistFile::string_type &file, __gnu_cxx::hash_map<PlaylistFile::string_type, PlaylistFile::string_type>& map){
    PlaylistFile::string_type::size_type pos = PlaylistFile::string_type::npos, prev_pos = 0, eq_pos = PlaylistFile::string_type::npos;
    // loop over every line and divide into (key, value) pairs separated by '='
    while ((pos = file.find('\n', prev_pos)) != PlaylistFile::string_type::npos){
	eq_pos = file.find('=', prev_pos);
	    
	if(eq_pos != PlaylistFile::string_type::npos && eq_pos < pos){
	    //~ std::cout << "  Key: |" << file.substr(prev_pos, eq_pos - prev_pos) << "|" << std::endl;
	    //~ std::cout << "  Value: %" << file.substr(eq_pos+1, pos - (eq_pos+1)) << "%" << std::endl;
	    map[file.substr(prev_pos, eq_pos - prev_pos)] = file.substr(eq_pos+1, pos - (eq_pos+1));
	}
	
	if (file.length() > (pos+1) && file[pos+1] == '\r')
	    pos += 2;
	else
	    pos++;
	
	prev_pos = pos;
    }
}
