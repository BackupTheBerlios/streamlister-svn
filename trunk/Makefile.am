bin_PROGRAMS = streamlister
streamlister_SOURCES = main.cxx dialog_boxes.cxx playlist_widgets.cxx playlist.cxx utility.cxx configuration.cxx player_window.cxx\
		 dialog_boxes.h playlist_widgets.h playlist.h utility.h configuration.h player_window.h
streamlister_CXXFLAGS = $(GTKMM_CFLAGS) $(LIBXMLPP_CFLAGS) $(CURLPP_CFLAGS) $(CFLAGS)
streamlister_LDFLAGS = $(GTKMM_LIBS) $(LIBXMLPP_LIBS) $(CURLPP_LIBS) $(LDFLAGS)

maintainer-config:
	aclocal && automake && autoconf

memusage-png:
	memusage -T -p streamlister-memusage.png streamlister

download_streamlisting:
	curl -0 -A 'User-Agent:Winamp/5.0' 'http://www.shoutcast.com/sbin/tvlister.phtml?limit=500&service=winamp2&no_compress=1' -o tvlisting.xml

changelog: svn2cl.xsl
	svn log -r 0:HEAD --xml -v > .cl.xml
	xsltproc -o ChangeLog svn2cl.xsl .cl.xml
	rm .cl.xml

#maintainer-clean-local: clean
super-clean: maintainer-clean
	rm -f aclocal.m4
	rm -f Makefile.in depcomp install-sh missing
	rm -f configure
	
	rm -f streamlister-memusage.png
