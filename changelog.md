<pre>

version 1.3.4

    removed wrong dk.po

version 1.3.3

    some small fixes and code cleanups

version 1.3.2

    support for custom page sizes
    new option "-F" to hide FileTab if file is given.
    Fix display of file arguments passed on the commandline.
    merge some small fixes
    fixed danish translation, thanks to scootergrisen
    fixed dutch translation, thanks to Pander

version 1.3.1

    spanish translation updated, thanks to Jose Luis Tirado
    new croatian translation, thanks to Zoran Spoja

version 1.3.0

    favorite printers patch, thanks to Amon Ott
    bugfixes and non-utf8-enhancements, thanks to Oleh Nykyforchyn
    fixes for autoconf/automake chain

version 1.2.10

    patch to work with AppArmor, thanks to Andreas Bolsch
    patch to compile against cups 1.6.0, thanks to Eric for mention these patches

version 1.2.9

    always use -lX11
    print jobs not shown in gtklpq
    setting port for gtklpq not possible
    compile with X11-Libs
    code cleanups

version 1.2.8a

    patched to compile with cups 1.5, thanks to Bengt Ahlgren

version 1.2.8

    remove "please wait", mentioned by Kees Lemmens
    new russian translations, thanks to Vyacheslav Dikonov

version 1.2.7

    new danish translation, thanks to Jørn Christensen.
    fixed "client not found" in gtklpq
    fixed compiler warnings
    removed support for Gtk1

version 1.2.6

    new ukrain translation, thanks to Mikhailo Lytvyn
    new czech translation, thanks to Marek Straka

version 1.2.5

    the accept/deny feature to hide printers isn't so much confusing anymore (at least i hope so...)
    if all printers are hidden the program shouldn't hang. 

version 1.2.4

    bigger "number of pages" box 

version 1.2.3

    ukrainian man-pages included, thanks to Lytvyn Mikhajlo
    translated man-pages now copied to docs
    new ukrainian translation, thanks to Lytvyn Mikhajlo
    collate bug fixed, thanks to Anders Blomdell

version 1.2.2

    language updates
    email in man-page has vanished
    UTF-Errors with GtkLPQ
    Patch to avoid dependencies to FreeType2

version 1.2.1

    BugFix: remembered printer is not suppressed by $PRINTER anymore
    new dutch translation

version 1.2.0

    GtkLPQ should now better handle long queues
    Progress-Bar when GtkLP starts (Gtk2 only)
    New option "save size and position" (Gtk2 only)
    polish translation updated

version 1.1.1

    gamma is now a function, so changed to gamma
    PPD-DropDowns should have no scrollbars anymore
    Gentoo-Patch in libgtklp (locale_h)
    new built script for packages
    some po-files changed

version 1.1.0

    new numbering scheme
    bug fix: number of pages in PPD-Sektion, too
    new gtk2 file selection dialog (optional)
    configure script changes

version 1.0g

    increased number of supported printers to 150 by default
    configure option added: --with-maxprinters=n

version 1.0f

    bugfix: number of copies changes via keyboard should work now

version 1.0e

    updated german translation

version 1.0d

    new chinese translations

version 1.0c

    "Double Free" bug fixed
    new polish translation
    backup files in source package removed

version 1.0b

    simple chinese translation added

version 1.0a

    new russian translation
    hungarian translation added
    fix for fixed console locales

version 1.0

    fixed configure scripts
    new italian translation
    new spanish translation
    new border-layout order

version 1.0pre3

    souped up Gtk2-Dialog-Boxes
    graphical error dialogs for "normal" errors
    option -i requires no argument anymore
    fixed double constrained checks
    better support for "enter" in dialogs
    bugs in password-dialog fixed
    icons in Gtk2

version 1.0pre2

    cosmetic in about box
    fixed some Copyright issues with GPL again.
    use g_warning instead of g_error to avoid core dumps.

version 1.0pre1

    fixed some licensing issues with GPL

version 0.9u/2

    brazilian translation, thanks to Marcelo Ricardo Leitner

version 0.9u

    delete chars from stdin on exit
    new configure option: --enable-forte
    PageSize is the same as PageRegion now.
    hopefully worked around the "pango error" on bad systems
    dont select whole directorys on print
    traditional chinese translation
    catalan translation updated

version 0.9s

    Collate Copies
    Image position is saved now
    frensh translation fixed
    russian translation updated

version 0.9r

    Gtk2 and Gtk1 support
    You can call gtklpq out of gtklp
    many bugs fixed
    Toggle TabView bug fixed
    File Selection dialog changed with gtk2
    Constraints check off by default
    new Strings, po-files need to be updated

version 0.9p

    Bug with german umlauts and frensh accents *should* be fixed, thanks to Bernhard Walle
    new frensh po-file, thanks to Roland Baudin
    crash when using templates fixed
    "remove all" added to file selection tab
    add button in file selection tab fixed
    added output-order option
    added print-mirror option

version 0.9n-2

    Segfault fixed

version 0.9n

    Fixes for Page Size/Region problems
    hide Printers, special thanks to Michael Münsch
    Layout optimizations
    PL-Lang updated
    some new icons

version 0.9m

    Bug with two tuxes in image pos selection fixed
    new italian translation, special thanks to Francesco Marletta
    GtkLPQ: reject/accept jobs
    give a reason when rejecting jobs, or disabling printers
    GtkLPG can act as accept, reject, enable, disable

version 0.9l

    autosize of fields with lpq-textmode. Nedded for some languages.
    some bug fixes in po-entries used by GtkLP *and* GtkLPQ
    GtkLP: print is default action
    GtkLPQ: cancel is default action
    image position has new design
    updated man-pages
    GtkLPQ can act as lprm when called like this
    timeout option for lpq

version 0.9k

    Warning when trying to compile with "without-cups" but should work on most circumstances
    number-up=(6|9|16) added
    numer-up-layout added
    page-border added
    textwrap adde
    landscape-mode with pictures

version 0.9i

    cyrillic language fixes
    multiselection mode for adding files
    russian translation fixed
    force console mode when called as lp*
    new environment variable GTKLP_FORCEX
    heavy compatibility improvements when called as lp*
    some new command-line options
    bugfix for -o name=value option
    new option "Save number of copies on exit"
    error codes fixed
    configfiles have changed
    error window not *behind* main window
    dialog windows are "always on top"
    error window is "always on top"

version 0.9g

    new frensh translation with special thanks to Roland Baudin
    new option -C (force console-mode)
    new option -h (help)

version 0.9f

    new configure and automake files with special thanks to Artur Frysiak
    old (incomplete) languages converted from Artur Frysiak
    japanese language support with special thanks to Takeshi AIHANA

version 0.9e

    italian support with special thanks to Francesco
    small cosmetic problems with GtkLPQ fixed
    GtkLPQ doesn't segfault in Debug-Mode on some systems anymore
    config.h.fallback included

version 0.9d

    configure support with special thanks to Nils
    gettext support with special thanks to Nils
    less segmentation faults
    some more user error`s
    some small bug fixes
    new option -g geometry for GtkLPQ
    new spec-file with special thanks to Nils

version 0.9

    GtkLP is now compatibel with lpoptions
    No problems with too many PPD-Entrys anymore
    console-support
    support for -o option=value at commandline
    Instance-Support
    Constraints-Support
    Icons for GtkLPQ
    New config-files

version 0.8h

    Remember last tab fixed

version 0.8g

    New Option: Remember last tab
    gtklprc changed
    lang-files changed

version 0.8f

    Italian support, some sizings fixed

version 0.8e

    Option sides=one-sided added for Duplex-Printers

version 0.8d

    Problems when no Description in PPD fixed
    Problems with foreign PPDs fixed
    Problems with QMS-PPDs fixed
    Message when wrong printer, program doesn`t exit.
    Problems when Printer removed on Server fixed.
    Number of Printers in Debug-Mode fixed

version 0.8c

    GtkLPQ fixed: It doesn't ran out of memory anymore

version 0.8b

    GtkLP fixed: Proplems with bad ppd`s
    Some cosmetic fixes

version 0.8a

    GtkLPQ fixed: Queue-update works again...

version 0.8

    password support implemented
    gtklpq, an queue tool added
    number of PPD-Entrys increased
    some bugs fixed...

version 0.6g

    Some Bugs fixed

version 0.6f

    Images next to the Duplex-Settings

version 0.6e

    new language-files
    some bugs fixed

version 0.6d

    new option -i (ignore stdin) to work with acroread
    stdin is ignored when a filename is given on command-line

version 0.6c

    ppd-bug fixed 

version 0.6b

    option -p port added
    bug with space at the end of OpenGroup-tags fixed 

version 0.6

    printer specific options from ppd included
    new config-files
    some bug fixed, some added ;-) 

version 0.4b

    trouble when only having one printer fixed

version 0.4a

    remember printer option fixed
    docs updated

version 0.4

    IPP Support for printing from cups clients
    use the cups librarys for printing
    new commandline option -S Servername for easily selecting other Servers than the default one
    new config files
    option "delete job after print" removed
    option "print jobid" added
    labels with line-breaks for Printer Informations

version 0.2

    Initial (alpha) Release 
  
</pre>
