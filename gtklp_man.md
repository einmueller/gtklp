<pre>
Man-Page for GtkLP


NAME
     gtklp - print files via CUPS

SYNOPSIS
     gtklp [-P|-d printer]  [-c  configdir]  [-S|-h  server]  [-p
     port] [-l] [-D] [-V] [-b] [-i] [-F] [-H] [-C] [-U user] [-#|
     -n n] [-E] [-q priority]  [-t|-J  jobname] [-o  option=value  
     ... ][file(s)]

     lp [-d printer] [-S|-h server] [-p port] [-D] [-V] [-b]  [-U
     user]  [-#|-n  n]  [-E]  [-q  priority]  [-t|-J jobname] [-P
     page-list [-H handling] [-X] [-o option=value ... ][file(s)]

     lpr [-P|-d printer] [-S|-h server] [-p port] [-D] [-V]  [-b]
     [-U user] [-#|-n n] [-E] [-q priority] [-t|-J|-C|-T jobname]
     [-X] [-o option=value ... ][file(s)]

DESCRIPTION
     GtkLP is an graphical frontend for  CUPS,  the  Common  UNIX
     Printing System.
     It is used to have an easy (and graphical) access to all  of
     the many options of cups-managed printers.

OPTIONS
     The following options are recognized by gtklp:

     -P printer
          Use printer as default destination.

     -d printer
          This option is equivalent to "-P".

     -c configdir
          Use configdir to set an alternative path to the config-
          files for GtkLP.

     -S server
          The name of the CUPS-Server.

     -h server
          The same as -S.

     -p port
          The port of CUPS-Server, 631 per default.

     -l
          Only  list  the   chosen   options   as   a   list   of
          commandline-parameters for lpr.
          Nice for scripting purposes...

     -D
          Debug.

     -V
          Displays the version of GtkLP.

     -b
          Use builtin defaults instead of saved options

     -i
          Ignore data on stdin.
          Sometimes useful, e.g. for printing from some  versions
          of the Acrobat Reader.

     -F   
          Hide the File-Tab if file is given at the command-line.

     -U user
          The name of the cups-User to use.

     -# n
          Make n copies per default.

     -n n
          The same as -#.

     -H
          Help

     -C
          Force console-mode

     -o option=value
          Specify extra options, same syntax as lpr

     -E
          Forces encryption when connecting to the server.

     -q priority
          Specify the priority of the print job, from 1  (lowest)
          to 100 (highest)

     -t jobname
          Specify the jobname

     -J jobname
          The same as -t.

options:
     If called as  lp,  the  following  options  differ  from  GtkLP`s

     -X   Force X-Mode, even if GTKLP_FORCEX is not set to 1

     -P page-list
          Specifies which pages to print in the   document.   The
          list   can   contain a list of numbers and ranges (#-#)
          separated by commas (e.g. 1,3-5,16).

     -H handling
          Specifies  when the job should be printed. A  value  of
          immediate  will  print the file immediately, a value of
          hold will hold the job indefinitely, and a  time  value
          (HH:MM)  will  hold the job until the  specified  time.
          (does this option work ?)

options:
     If called as lpr,  the  following  options  differ  from  GtkLP`s

     -X   Force X-Mode, even if GTKLP_FORCEX is not set to 1

     -C jobname
          Specify the jobname

     -T jobname
          The same as -C.

     -p   Specifies that the print  file  should   be   formatted
          with   a   shaded header with the date, time, job name,
          and  page  number.  This  option  is   equivalent    to
          "-oprettyprint"  and  is only useful when printing text
          files.

     -l   Specifies that the print file  is   already   formatted
          for   the   destination  and  should  be  sent  without
          filtering. This option is equivalent to "-oraw".


     Some other options from systemv`s lp and berkeley`s lpr  are
     ignored without error message

PATHS
     GtkLP looks for config-files under
     <configdir>
     then under
     $HOME/.gtklp/
     and at last under
     /etc/gtklp/.

     To hide Printers from users, use the following pathes:
     /etc/gtklp/accept/
     /etc/gtklp/deny/
     $HOME/.gtklp/

     Just  put  an  empty  file  with  the  name of the printer the user should see by
     default to accept, put an empty file with the name of the printer the user should
     never  see  to  deny.   If  only accept exists, all but the accepted printers are
     allowed.  If only deny exists, all but the denied printers are accepted.

     All users can deny accepted printers, but they can't unhide denied ones.


SPECIAL
     In GtkLP you can define a browser to open the help URL,  the
     CUPS-Homepage  and  so  on.  Just use the your favorite HTML
     browser. The first $1 in command-line will be substituted by
     the URL.

     If  you  can't  connect  to  an  X-Server,  GtkLP  acts   as
     console-lpr.

     If called as lpr, or lp, GtkLP acts on console and has  some
     other command-line parameters, see above.

VARIABLES
     You can set the following environment variables to influence
     the behaviour of GtkLP:

     CUPS_SERVER
          The default CUPS-server to connect to

     IPP_PORT
          The default Port of the CUPS-server to connect to

     PRINTER
          The default printer to use

     GTKLP_FORCEX
          Set to 1 to force X-Mode for lpr and lp

ERROR
     GtkLP produces the following Error-Codes:

     0    normal program termination

     1    small error, wrong parameters, etc.

     2    Server Problems, server not found, no  default  printer
          found, etc.

     3    BAD  error.  Fields  declared  too  short,   too   many
          printers, etc.

SEE ALSO
     gtklpq(1), lpr(1), CUPS Software Users Manual
     http://localhost:631/documentation.html

COPYRIGHT
     This program was written by T. Mueller, 2000, 2001, 2002.
     It is licensed under the Terms of  the  GNU  GENERAL  PUBLIC
     LICENSE,
     see also http://www.gnu.org.


     </pre>

