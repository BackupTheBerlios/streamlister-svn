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

#include "utility.h"

size_t write_data(char *buffer, size_t size, size_t nitems, void *instream){
    //~ std::cout << "B: write_data()" << std::endl;
    std::string &obuf = *((std::string*)instream);
    //~ std::cout << "obuf.length() = " << obuf.length() << std::endl;
    //~ std::cout << "size*nitems = " << (size*nitems) << std::endl;
    obuf.append(buffer, size*nitems);
    //~ std::cout << "E: write_data()" << std::endl;
    return size*nitems;
}
