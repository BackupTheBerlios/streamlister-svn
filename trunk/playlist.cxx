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

#include "playlist.h"

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
std::string PlaylistEntry::get_data(const std::string& which) const {
    std::string retstr("");
    if (std::string("Name") == which){
	retstr = name;
    }else if(std::string("StationID") == which){
	retstr = stationid;
    }else if(std::string("Genre") == which){
	retstr = genre;
    }else if(std::string("Nowplaying") == which){
	retstr = nowplaying;
    }else if(std::string("Listeners") == which){
	retstr = listeners;
    }else if(std::string("Bitrate") == which){
	retstr = bitrate;
    }else if(std::string("Rating") == which){
	retstr = rating;
    }else if(std::string("Playstring") == which){
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

Playlist::Playlist(const std::string &file){
    std::cout << "Playlist::Playlist()" << std::endl;
    m_filename = file;
    
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

bool Playlist::parse_file(const std::string & file){
    
    std::string filename(file);
    if (filename == ""){
	if (m_filename != "")
	    filename = m_filename;
	else
	    return false;
    }
    
    std::cout << "Parsing file: " << filename << std::endl;
    
    xmlpp::DomParser tvlist_xml_doc(filename, false);
    
    xmlpp::Document *doc = tvlist_xml_doc.get_document();
    
    xmlpp::Element *cur_element = doc->get_root_node();
    
    std::cout << "path: " << cur_element->get_path() << std::endl;
    
    xmlpp::Node::NodeList playlists = cur_element->get_children(std::string("playlist"));
    
    if (playlists.size() != 1){
	std::cout << "0 or more than 1 playlists: " << playlists.size() << std::endl;
	return false;
    }
    
    // points to playlist element
    cur_element = static_cast<xmlpp::Element *>(*(playlists.begin()));
    
    xmlpp::Element::AttributeList attrlist = cur_element->get_attributes();
    
    xmlpp::Element::AttributeList::iterator i = attrlist.begin();
    for(;i != attrlist.end(); i++){
	std::cout << "Name: " << (*i)->get_name() << ", Value: " << (*i)->get_value() << std::endl;
    }
    
    xmlpp::NodeSet nset = cur_element->find(std::string("entry"));
    
    __gnu_cxx::hash_set<std::string, __gnu_cxx::hash<std::string> > genre_hashset;
    
    // loop over all entry nodes, putting them into antry list, aand making hash_set of genres
    for(xmlpp::NodeSet::iterator j = nset.begin(); j != nset.end(); j++){
	m_entries.push_back(PlaylistEntry((static_cast<xmlpp::Element *>(*j))));
	//~ std::cout << entries.rbegin()->get_genre() << std::endl;
	genre_hashset.insert(m_entries.rbegin()->get_data("Genre"));
	//~ std::cout << *(entries.rbegin());
    }
    
    // now put into vector sorted
    //~ std::vector<std::string> genre_list_unsorted;
    //~ for(__gnu_cxx::hash_set<std::string>::iterator i = genre_hashset.begin();
	//~ i != genre_hashset.end(); genre_list_unsorted.push_back(*i), i++);
    m_genre_list.resize(genre_hashset.size());
    std::partial_sort_copy<__gnu_cxx::hash_set<std::string, __gnu_cxx::hash<std::string> >::iterator,
		      std::vector<std::string>::iterator>
	(genre_hashset.begin(), genre_hashset.end(),
	 m_genre_list.begin(), m_genre_list.end());
    
    //~ std::cout << "genre_list.size() = " << genre_list.size() << std::endl;
    //~ for(std::vector<std::string>::iterator i = genre_list.begin(); i != genre_list.end(); i++){
	//~ std::cout << *i << std::endl;
    //~ }
    
    return true;
}

//~ const std::vector<std::string>& Playlist::get_genres() const{
    //~ return genre_list;
//~ }

//~ const std::vector<Glib::ustring>& Playlist::get_properties() const {
    //~ return properties;
//~ }

/*** class PlaylistFile ***/

PlaylistFile::PlaylistFile(const Glib::ustring &plsfile):numberofentries(0),version(0){
    parse_file(plsfile);
}

// FIXME: this could be better
void PlaylistFile::parse_file(const Glib::ustring &plsfile){
    // parse the pls file
    if (plsfile.compare(0, 10, "[playlist]") == 0){
	std::cout << "found playlist header" << std::endl;
	
	__gnu_cxx::hash_map<Glib::ustring, Glib::ustring> map;
	
	_parse_equals(plsfile.substr(10), map);
	
	__gnu_cxx::hash_map<Glib::ustring, Glib::ustring>::iterator iter = map.find("numberofentries");
	if(iter != map.end()){
	    numberofentries = atoi((iter->second).c_str());
	    
	    PlaylistFileDesc pls_desc;
	    Glib::ustring usFile("File_"), usTitle("Title_"), usLength("Length_");
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
		    std::cout << "+++ERROR: could not find key: " << usFile << std::endl;
		
		iter = map.find(usTitle);
		if(iter != map.end()){
		    pls_desc.title = (*iter).second;
		}else
		    std::cout << "+++ERROR: could not find key: " << usTitle << std::endl;
		
		iter = map.find(usLength);
		if(iter != map.end()){
		    pls_desc.length = atoi((iter->second).c_str());
		}else
		    std::cout << "+++ERROR: could not find key: " << usLength << std::endl;
		
		files.push_back(pls_desc);
	    }
	    
	    if((iter = map.find("Version")) != map.end()){
		version = atoi((iter->second).c_str());
	    }else
		std::cout << "+++ERROR: could not find 'Version'" << std::endl;
	    
	}else
	    std::cout << "+++ERROR: could not find 'numberofentries'" << std::endl;
	
    }else{
	// throw exception
	
	std::cout << "+++ERROR: could not find playlist header" << std::endl;
    }
    
}

//~ const std::vector<PlaylistFileDesc>& PlaylistFile::get_files() const {
    //~ return files;
//~ }

std::vector<Glib::ustring> PlaylistFile::get_titles() const{
    std::vector<Glib::ustring> titles;
    for(std::vector<PlaylistFileDesc>::const_iterator i = files.begin(); i != files.end(); i++){
	titles.push_back(i->title);
    }
    return titles;
}

//~ size_type PlaylistFile::size() const { return files.size(); }

void PlaylistFile::_parse_equals(const Glib::ustring &file, __gnu_cxx::hash_map<Glib::ustring, Glib::ustring>& map){
    std::string::size_type pos = std::string::npos, prev_pos = 0, eq_pos = std::string::npos;
    // loop over every line and divide into (key, value) pairs separated by '='
    while ((pos = file.find('\n', prev_pos)) != std::string::npos){
	eq_pos = file.find('=', prev_pos);
	    
	if(eq_pos != std::string::npos && eq_pos < pos){
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
