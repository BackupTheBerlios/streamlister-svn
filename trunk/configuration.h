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

//~ #include <map>
#include <vector>
#include <utility>
#include <stdexcept>

#include <libxml++/libxml++.h>
#include <gtkmm.h>

#include "utility.h"

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

class Configuration{
    public:
	typedef Glib::ustring string_type;
	typedef std::pair<string_type, bool> item_type;
	typedef std::vector<item_type> itemlist_type;
    
    public:
	Configuration(const string_type &file = string_type());
	
	void parse_config(const string_type &file = string_type());
	//~ void save_config(const string_type &file = string_type());
	void save();
	
	/* Getters */
	string_type get_configfile() const {return m_configfile;}
	string_type get_player_cmd() const {return m_player_cmd;}
	string_type get_url() const {return m_url;}
	string_type get_num_entries() const {return m_num_entries;}
	bool get_cache_pls() const {return m_cache_pls;}
	const itemlist_type& get_columns() const {return m_columns;}
	const itemlist_type& get_ratings() const {return m_ratings;}
	
	/* Setters */
	void set_configfile(const string_type &s){m_configfile = s;}
	void set_player_cmd(const string_type &s){m_player_cmd = s;}
	void set_url(const string_type &s){m_url = s;}
	void set_num_entries(const string_type &s){m_num_entries = s;}
	void set_cache_pls(bool b){m_cache_pls = b;}
	void set_columns(itemlist_type);
	void set_ratings(itemlist_type);
	
	//~ Glib::ustring operator [](const string_type& k){
	    
	    //~ return configuration_data[k];
	//~ }
    
    private:
	void _parse_player_cmd(xmlpp::Element * const);
	void _parse_url(xmlpp::Element * const);
	void _parse_cache_pls(xmlpp::Element * const);
	void _parse_columns(xmlpp::Element * const);
	void _parse_ratings(xmlpp::Element * const);
    
    private:
	static void (Configuration::*_parse_fun_array[])(xmlpp::Element *);
    
    private:
	string_type m_configfile;
	string_type m_player_cmd;
	string_type m_url;
	string_type m_num_entries;
	
	bool m_cache_pls;
	
	itemlist_type m_columns;
	itemlist_type m_ratings;
	//~ __gnu_cxx::hash_set<string_type> columns;
	//~ __gnu_cxx::hash_set<string_type> ratings;
	
	//~ std::map<string_type, string_type> configuration_data;
    
	xmlpp::DomParser             m_xmldocument;
};

#endif
