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

#define CURL_SYSTEM "curl -0 -A 'User-Agent:Winamp/5.0' 'http://www.shoutcast.com/sbin/tvlister.phtml?limit=500&service=winamp2' -o nsv_tvlisting"
//~ #define SHOUTCAST_URL "http://www.shoutcast.com/sbin/tvlister.phtml?limit=500&service=winamp2"
#define SHOUTCAST_URL "http://www.shoutcast.com/sbin/tvlister.phtml?limit=%s&service=winamp2&no_compress=1"
#define PLAYER_CMD "mplayer -v -nocache '%s'"
#define DEFAULT_NUMBER_ENTRIES "500"
#define DEFAULT_CONFIG_PATH "~/.streamlisterrc"
#define DEFAULT_CACHE_PLS false

#include <cassert>

#include "configuration.h"

static void __parse_ordered_visibility_list(std::vector<std::pair<Glib::ustring, bool> > &, xmlpp::Element * const, const Glib::ustring &);

enum {VIDEO_PLAYER, URL, CACHE_PLS, COLUMNS, RATINGS, TOTAL_NUM};

void (Configuration::*Configuration::_parse_fun_array[TOTAL_NUM])(xmlpp::Element * const) = {
    &Configuration::_parse_player_cmd,
    &Configuration::_parse_url,
    &Configuration::_parse_cache_pls,
    &Configuration::_parse_columns,
    &Configuration::_parse_ratings
};

Configuration::Configuration(const Glib::ustring &file)
    /* set defaults */
    :m_configfile((file != "")?file:DEFAULT_CONFIG_PATH),
     m_player_cmd(PLAYER_CMD),m_url(SHOUTCAST_URL),
     m_num_entries(DEFAULT_NUMBER_ENTRIES),m_cache_pls(DEFAULT_CACHE_PLS)
{
    dout(1) << "Configuration::Configuration(" << file << ")" << std::endl;
    if(m_configfile != ""){
	parse_config(m_configfile);
    }
    
    dout(8) << "===========================" << std::endl;
    dout(8) << "m_configfile: " << m_configfile << std::endl;
    dout(8) << "m_player_cmd: " << m_player_cmd << std::endl;
    dout(8) << "m_url: " << m_url << std::endl;
    dout(8) << "m_num_entries: " << m_num_entries << std::endl;
    dout(8) << "m_cache_pls: " << m_cache_pls << std::endl;
    dout(8) << "m_columns: " << std::endl;
    for(std::vector<std::pair<string_type, bool> >::iterator i = m_columns.begin(); i != m_columns.end(); i++){
	dout(8) << "  " << i->first << " (" << ((i->second)?"show":"hide") << ")" << std::endl;
    }
    dout(8) << "m_ratings: " << std::endl;
    for(std::vector<std::pair<string_type, bool> >::iterator i = m_ratings.begin(); i != m_ratings.end(); i++){
	dout(8) << "  " << i->first << " (" << ((i->second)?"show":"hide") << ")" << std::endl;
    }
    dout(8) << "===========================" << std::endl;
}

void Configuration::parse_config(const Glib::ustring &file){
    dout(1) << "Configuration::parse_config(" << file << ")" << std::endl;
    
    if (file == ""){
	if (m_configfile == "")
	    throw std::runtime_error("No configfile specified");
    }else{
	m_configfile = file;
    }
    
    /* check to see if configfile exists, if not use defaults */
    if(!file_exists(m_configfile)){
	return;
    }
    
    if(m_configfile.substr(0, 2) == string_type("~/")){
	m_configfile = string_type(getenv("HOME")) + file.substr(1);
    }
    
    dout(7) << "Parsing configfile: " << m_configfile << std::endl;
    
    m_xmldocument.parse_file(m_configfile);
    
    //~ xmlpp::Document *doc = m_xmldocument.get_document();
    //~ xmlpp::Element *cur_element = m_xmldocument.get_document()->get_root_node();
    xmlpp::Node::NodeList config_nodes = m_xmldocument.get_document()->get_root_node()->get_children();
    
    xmlpp::Node::NodeList::iterator i = config_nodes.begin();
    int j = VIDEO_PLAYER;
    for(; i != config_nodes.end(); i++){
	if(dynamic_cast<xmlpp::Element*>(*i)){
	    dout(8) << "See Element: " << (*i)->get_name() << std::endl;
	    assert(j < TOTAL_NUM);
	    (this->*(_parse_fun_array[j]))(dynamic_cast<xmlpp::Element*>(*i));
	    j++;
	}
    }
    return;
    //~ for(int i = VIDEO_PLAYER; i < TOTAL_NUM; i++){
	//~ _parse_fun_array[i](config_nodes[i]);
    //~ }
}

//~ void Configuration::save_config(const Glib::ustring &file){
void Configuration::save(){
    dout(3) << "Configuration::save(): " << m_configfile << std::endl;
    
}

/*****                                                                                                                   *****/
/***** Private memeber functions for parsing different config tags *****/
/*****                                                                                                                   *****/
void Configuration::_parse_player_cmd(xmlpp::Element * const elem){
    dout(7) << "Configuration::_parse_player_cmd" << std::endl;
    dout(9) << "name: " << elem->get_name() << std::endl;
    if(elem->has_child_text())
	m_player_cmd = elem->get_child_text()->get_content();
}

void Configuration::_parse_url(xmlpp::Element * const elem){
    dout(7) << "Configuration::_parse_url" << std::endl;
    if(elem->has_child_text())
	m_url = elem->get_child_text()->get_content();
    
    xmlpp::Attribute *attr = elem->get_attribute("num_entries");
    if(attr)
	m_num_entries = attr->get_value();
}

void Configuration::_parse_cache_pls(xmlpp::Element * const elem){
    dout(7) << "Configuration::_parse_cache_pls" << std::endl;
    if(elem->has_child_text())
	m_cache_pls = (elem->get_child_text()->get_content() == "yes")?true:false;
}

void Configuration::_parse_columns(xmlpp::Element * const elem){
    dout(7) << "Configuration::_parse_columns" << std::endl;
    __parse_ordered_visibility_list(m_columns, elem, "col");
}

void Configuration::_parse_ratings(xmlpp::Element * const elem){
    dout(7) << "Configuration::_parse_ratings" << std::endl;
    __parse_ordered_visibility_list(m_ratings, elem, "rating");
}

/***** HELPER FUNCTIONS *****/
void __parse_ordered_visibility_list(std::vector<std::pair<Glib::ustring, bool> > &vec, xmlpp::Element * const elem, const Glib::ustring &col_name){
    dout(7) << "__parse_ordered_visibility_list" << std::endl;
    xmlpp::Element *col_elem = NULL;
    xmlpp::Node::NodeList col_nodes = elem->get_children();
    for(xmlpp::Node::NodeList::iterator i = col_nodes.begin(); i != col_nodes.end(); i++){
	if((*i)->get_name() == col_name && (col_elem = dynamic_cast<xmlpp::Element*>(*i))){
	    xmlpp::Attribute *name_attr = col_elem->get_attribute("name");
	    if(name_attr){
		xmlpp::Attribute *vis_attr = col_elem->get_attribute("visibility");
		vec.push_back(std::make_pair(name_attr->get_value(),(vis_attr)?((vis_attr->get_value() == "show")?true:false):true));
	    }
	}
    }
}
