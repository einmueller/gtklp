Summary: a gtk frontend to CUPS
Name: gtklp
Version: 1.3.4
Release: 1
License: GPL
Vendor: SirTobi
Group: Utilities/Printing
Source: http://belnet.dl.sourceforge.net/sourceforge/gtklp/gtklp-%{version}.src.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
BuildPrereq: cups-devel > 1.1 gtk+-devel
Requires: cups > 1.1
%description
a gtk frontend to CUPS

%prep
%setup

%build
./configure --prefix=%{_prefix}
make

%install
rm -rf %{buildroot}
make install prefix=%{buildroot}%{_prefix}
strip %{buildroot}%{_prefix}/bin/* || :
mkdir -p %{buildroot}%{_docdir}/gtklp-%{version}
cp AUTHORS BUGS ChangeLog COPYING NEWS README README.Irix TODO USAGE %{buildroot}%{_docdir}/gtklp-%{version}/

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%{_prefix}

%changelog
* Fr Nov 01 2019 Tobias Mueller <gtklp@sirtobi.com>
- version 1.3.4
- removed wrong dk.po

* Mon Mar 06 2017 Tobias Mueller <gtklp@sirtobi.com>
- version 1.3.3
- code cleanups and small fixes

* Sun Mar 05 2017 Tobias Mueller <gtklp@sirtobi.com>
- version 1.3.2
- support for custom page sizes
- new option "-F" to hide FileTab if file is given.
- Fix display of file arguments passed on the commandline.
- merge some small fixes
- fixed danish translation, thanks to scootergrisen
- fixed dutch translation, thanks to Pander

* Wed Feb 26 2014 Tobias Mueller <gtklp@sirtobi.com>
- version 1.3.1
- spanish translation updated, thanks to Jose Luis Tirado
- new croatian translation, thanks to Zoran Spoja

* Tue Mar 12 2013 Tobias Mueller <gtklp@sirtobi.com>
- version 1.3.0
- favorite printers patch, thanks to Amon Ott
- bugfixes and non-utf8-enhancements, thanks to Oleh Nykyforchyn
- fixes for autoconf/automake chain

* Mon Oct 22 2012 Tobias Mueller <gtklp@sirtobi.com>
- version 1.2.10
- patch to work with AppArmor, thanks to Andreas Bolsch
- patch to compile against cups 1.6.0, thanks to Eric

* Wed May 16 2012 Tobias Mueller <gtklp@sirtobi.com>
- version 1.2.9
- always use -lX11
- print jobs not shown in gtklpq
- setting port for gtklpq not possible
- compile with X11-Libs
- code cleanups

* Mon Mar 12 2012 Tobias Mueller <gtklp@sirtobi.com>
- version 1.2.8a
- patch to compile with cups 1.5, thanks to Bengt Ahlgren 

* Thu Feb 11 2010 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 1.2.8
- remove "waiting" window
- new russian translation

* Sun Jun 21 2009 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 1.2.7
- danish translation
- fixed "client not found" in gtklpq
- fixed compiler warnings

* Thu Nov 15 2007 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 1.2.6
- new ukrain translation, thanks to Mikhailo Lytvyn
- new czech translation, thanks to Marek Straka

* Thu Aug 30 2007 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 1.2.5
- made accept/deny mechanism a bit more logical
- no hangup when all printers are denied

* Tue Jul 31 2007 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 1.2.4
- bigger drop down for "number of pages"

* Thu Feb 08 2007 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 1.2.3
- ukrain man-pages included, thanks to Lytvyn Mikhajlo 
- translated man-pages now copied to docs.
- new ukrain translation, thanks to Lytvyn Mikhajlo
- collate bug fixed, thanks to Anders Blomdell

* Wed May 31 2006 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 1.2.2 
- language updates
- email in man-page has vanished
- UTF-Errors with GtkLPQ
- Patch to avoid dependencies to FreeType2

* Sun Feb 26 2006 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 1.2.1
- new dutch translation
- $PRINTER is overwritten by "remember printer"

* Sun Feb 19 2006 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 1.2.0
- new gtklp option: remember size and position (gtk2)
- progress Bar on startup (gtk2)
- gtklpq should better react with many jobs
- new polish translation
- "Please wait" shown on slow systems too ?

* Thu Feb 14 2006 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 1.1.1
- gamma is now a function, so changed to gamma
- PPD-DropDowns should have no scrollbars anymore
- Gentoo-Patch in libgtklp (locale_h)
- new built script for packages
- some po-files changed

* Fri Nov 12 2005 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- new version numbering scheme
- ignore Number of Copies in PPD-File
- Change address of the FSF in Source-Files
- new file chooser dialog (optional)
- some changes in configure script

* Thu Jul 28 2005 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- support for 150 printers per default
- new configure option: --with-maxprinters=n
- some more READMEs

* Sun Jun 18 2005 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- fixed bug when changing number of copies via keyboard

* Sat Jun 18 2005 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- new german translation

* Thu May 05 2005 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- new chinese translations

* Thu Mar 03 2005 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- new polish translations
- "double free()" bug fixed

* Mon Jan 03 2005 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- simple chinese translation

* Wed Dec 23 2004 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- new russion translation
- fixed console locales
- new translation in hungarian.

* Wed Nov 17 2004 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- small fixes in conbfigure.in
- new italian translation
- new order of border buttons
- new spanish translation

* Sun Oct 03 2004 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- souped up Gtk2-Dialog-Boxes
- graphical error dialogs for "normal" errors
- option -i requires no argument anymore
- fixed double constrained checks
- better support for "enter" in dialogs
- bugs in password-dialog fixed
- icons in Gtk2

* Sun Aug 30 2004 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- cosmetic in about box
- fixed some Copyright issues with GPL.
- use g_warning instead of g_error to avoid core dumps.

* Sun Aug 29 2004 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- fixed some Copyright issues with GPL.

* Fri Jul 30 2004 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- slackware packages built using makepkg
- brazilian language support adden, thanks to Marcelo Ricardo Leitner

* Wed Jul 26 2004 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.9u
- delete chars from stdin on exit
- new configure option: --enable-forte
- PageSize is the same as PageRegion now.
- hopefully worked around the "pango error" on bad systems
- dont select whole directorys on print
- traditional chinese translation
- catalan translation updated

* Mon May 17 2004 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.9s
- New option: collate copies
- frensh po updated
- image position fixed

* Sun May 09 2004 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.9r
- Toggle TabView bug fixed
- Spanish translation fixed
- man-pages fixed
- introducing gtk2 support
- File Selection dialog changed with gtk2
- Catalan translation added
- Constraints check off by default
- new Strings, po-files need to be updated
- ability to call gtklpq out of gtklp


* Sun Aug 03 2003 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.9p
- Bug with german umlauts and frensh accents *should* be fixed, thanks to Bernhard Walle
- new frensh po-file, thanks to Roland Baudin
- crash when using templates fixed
- "remove all" added to file selection tab
- add button in file selection tab fixed
- added output-order option
- added print-mirror option

* Sun May 18 2003 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.9n
- option to hide printers to the users
- updated of pl.po
- Bug Fix for the "Page Region/Size" Problem
- Some Layout changes
- some new icons

* Sun Sep 01 2002 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.9m
- Bug with two tuxes in image pos selection fixed
- new italian translation
- GtkLPQ: reject/accept jobs
- give a reason when rejecting jobs, or disabling printers
- GtkLPG can act as accept, reject, enable, disable

* Sun Aug 25 2002 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.9l
- autosize of fields with lpq-textmode. Nedded for some languages.
- some bug fixes in po-entries used by GtkLP *and* GtkLPQ
- GtkLP: print is default action
- GtkLPQ: cancel is default action
- image position has new design
- updated man-pages
- GtkLPQ can act as lprm when called like this
- timeout option for lpq

* Sun Jul 07 2002 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.9k
- Warning when trying to compile with "without-cups" but should work on most circumstances
- number-up=(6|9|16) added 
- numer-up-layout added
- page-border added
- textwrap added
- landscape-mode with pictures

* Mon May 20 2002 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.9i
- cyrillic language fixes
- multiselection mode for adding files
- russian translation fixed
- force console mode when called as lp*
- new environment variable GTKLP_FORCEX
- heavy compatibility improvements when called as lp*
- some new command-line options
- bugfix for -o name=value option
- new option "Save number of copies on exit"
- error codes fixed
- configfiles have changed
- error window not *behind* main window
- dialog windows "always on top"

* Wed Apr 17 2002 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.9g
- frensh translation fixed by Roland Baudin <Roland.Baudin@space.alcatel.fr>
- New Option -C
- New Option -h

* Thu Apr 04 2002 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.9f
- new configure and automake files from Artur Frysiak <wiget@pld.org.pl>
- old (incomplete) languages converted from Artur Frysiak <wiget@pld.org.pl>
- japanese language support from Takeshi AIHANA <aihana@jcom.home.ne.jp>
 
* Thu Mar 28 2002 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.9e
- gtklpq -D should work on all version for now
- fixed italian translation from Francesco Marletta <fmarletta@diit.unict.it>

* Wed Mar 27 2002 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.9d
- configure support
- gettext support
- less segmentation faults
- more user errors
- new option -g geometry for gtklpq
- new spec file

* Wed Mar 06 2002 Nils Philippsen <nils@redhat.de>
- version 0.9b
- fix spec file

* Sun Mar 06 2002 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.9a
- bug fixes in GtkLPQ

* Tue Mar 05 2002 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.9
- compatibel with lpoptions
- problem with too many PPD-Entrys fixed
- console support
- support for -o option=value at commandline
- support for instances
- support for constraints
- icons for GtkLPQ
- new config files

* Thu Nov 11 2001 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.8h
- remember last tab fixed

* Tue Oct 30 2001 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.8g
- new option: remember last tab
- gtklprc changed
- lang-files changed

* Fri Oct 26 2001 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.8f
- italian support
- some sizings fixed

* Wed Oct 24 2001 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.8e
- option sides=one-sided added

* Tue Oct 16 2001 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.8d
- problems when no description in PPD fixed
- problems with foreign PPDs fixed
- problems with QMS-PPDs fixed
- message when wrong printer, program doesn`t exit
- problems when Printer removed on server fixed
- number of printers in debug-mode fixed

* Thu Sep 27 2001 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.8c
- GtkLPQ doesn`t ran out of memory anymore

* Thu Sep 13 2001 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.8b
- problem with bad ppd`s fixed
- some cosmetic fixes

* Wed Sep 12 2001 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.8a
- queue-update in GtkLPQ works again

* Thu Sep 06 2001 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.8
- password support implemented
- gtklpq, an queue tool added
- number of PPD-Entrys increased

* Thu May 03 2001 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.6g
- bug fixes

* Thu Mar 01 2001 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.6f
- images next to duplex settings

* Sat Nov 25 2000 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.6e
- new languages

* Fri Nov 24 2000 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.6d
- new option -i (ignore stdin)
- stdin ignored when filename given

* Fri Nov 10 2000 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.6c
- ppd-bug fixed

* Wed Oct 25 2000 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.6b
- option -p port added
- bug with space at the end of OpenGroup-tags fixed

* Mon Oct 23 2000 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.6
- printer specific options from ppd included
- new config-files

* Wed Aug 30 2000 Tobias Mueller <sirtobi@stud.uni-hannover.de>
- version 0.4b
- trouble when only having one printer fixed

- version 0.4a
- remember printer option fixed
- docs updated

- version 0.4
- IPP Support for printing from cups clients
- use the cups librarys for printing
- new commandline option -S Servername for easily selecting other Servers than the default one
- new config files
- option "delete job after print" removed
- option "print jobid" added
- labels with line-breaks for Printer Informations

- version 0.2
- initial (alpha) release







