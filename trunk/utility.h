
#include <string>

#if __GNUC__ == 3 && __GNUC_MINOR__ < 4
#  include <ext/stl_hash_fun.h>
#else
#  include <ext/hash_fun.h>
#endif

#include <glibmm.h>

#ifndef __UTILITY_H__
#define __UTILITY_H__

/*** Make hash functor for hashing std::strings  ***/
namespace __gnu_cxx
{
    template<> struct hash<std::string> {
      size_t operator()(std::string __x) const { return __gnu_cxx::hash<const std::string::value_type*>()(__x.c_str()); }
    };
    template<> struct hash<Glib::ustring> {
      //~ size_t operator()(Glib::ustring __x) const { return __gnu_cxx::hash<const Glib::ustring::value_type*>()(__x.c_str()); }
      size_t operator()(Glib::ustring __x) const { return __gnu_cxx::hash<const char*>()(__x.c_str()); }
    };
    //~ template<> struct hash<const gunichar*> {
      //~ size_t operator()(const gunichar* __x) const { return __gnu_cxx::hash<const char*>()(__x.c_str()); }
    //~ };
} // END namespace __gnu_cxx

enum {LEFT_MOUSE_BUTTON=1, MIDDLE_MOUSE_BUTTON=2, RIGHT_MOUSE_BUTTON=3};

//~ template <class T = int, int S = 0>
//~ void test_func(T arg = S){
    //~ std::cout << "test_func(): " << arg << std::endl;
    //~ std::cout << "int(): " << int() << std::endl;
//~ }

#if 0
template <class T, const char *S = "%s">
#else
template <class T>
#define S "%s"
#endif
T string_subst(const T& str, const T& replace, const T& search=T(S)){
    typename T::size_type pos = 0;
    T ret_str(str);
    while((pos = ret_str.find(search, pos)) != T::npos){
	ret_str.replace(pos, search.size(), replace);
    }
    //~ std::cout << "string_subst(): " << ret_str << std::endl;
    return ret_str;
}
#undef S

#endif /* __UTILITY_H__ */
