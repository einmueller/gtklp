<pre>

Man-Page for GtkLPQ


NAME
     gtklpq - manage print queues via CUPS

SYNOPSIS
     gtklpq [-P|-d printer] [-S server] [-p port] [-D]  [-V]  [-U
     user] [-t timeout] [-g XxY] [-C] [-h] [-E]

     lpq [-P|-d printer] [-S server]  [-p  port]  [-D]  [-V]  [-U
     user] [-t timeout] [-E]

     lprm [-P|-d printer] [-S server] [-p  port]  [-D]  [-V]  [-U
     user] [-E] [-] [job ID(s)]

     reject [-h server] [-p port] [-D] [-V] [-U  user]  [-E]  [-r
     reason] [destination(s)]

     accept [-h server] [-p port] [-D] [-V] [-U  user]  [-E]  [-r
     reason] [destination(s)]

     disable [-h server] [-c] [-p port] [-D] [-V] [-U user]  [-E]
     [-r reason] [destination(s)]

     enable [-h server] [-p port] [-D] [-V] [-U  user]  [-E]  [-r
     reason] [destination(s)]

DESCRIPTION
     GtkLPQ is an graphical frontend for CUPS,  the  Common  UNIX
     Printing System.
     It is used to manage CUPS-Printqueues.

     GtkLPQ
          Graphical Frontend

     lpq
          Show print queue

     lprm
          Remove jobs

     disable/enable
          Disable/enable printer

     reject/accept
          Reject/Accept printjobs for this printer

     GtkLPQ is part of the GtkLP-package.

OPTIONS
     The following options are recognized by gtklpq:

     -P printer
          Use printer as default destination.

     -d printer
          This option is equivalent to "-P".

     -S server
          The name of the CUPS-Server.

     -p port
          The port of CUPS-Server, 631 per default.

     -D
          Debug.

     -V
          Displays the version of GtkLPQ.

     -U user
          The name of the cups-User to use.

     -t timeout
          The intervall in seconds between two queue-updates.

     -g XxY
          Pop up GtkLPQ at position (X,Y).

     -h
          Help

     -C
          Force console-mode.

     -E
          Forces encryption when connecting to the server.

options:
     If called as lpq, the  following  options  differ  from  GtkLPQ`s

     -X
          Force X-Mode, even if GTKLP_FORCEX is not set to 1

options:
     If called as lprm, the following  options  differ  from  GtkLPQ`s

     -
          Cancel all jobs

     Without any given jobs, lprm removes the current job.

options:
     If called as disable, the following options differ from  GtkLPQ`s

     -c
          Cancel all jobs

     -r reason
          Give an reason, why you do so


options:
     If called as reject, the following options differ  from  GtkLPQ`s

     -r reason
          Give an reason, why you do so

PATHS
     To hide Printers from users, use the following pathes:
     /etc/gtklp/accept/
     /etc/gtklp/deny/
     $HOME/.gtklp/

     Just put an empty file with the name of the printer the user
     should  see by default to accept, put an empty file with the
     name of the printer the user should never see to deny.
     The accept values can be overwritten by the user,  the  deny
     values can't.

SPECIAL
     If  you  can`t  connect  to  an  X-Server,  GtkLPQ  acts  as
     console-lpq.

     If called as lpq, enable, disable, accept, or reject, GtkLPQ
     acts  on  console  like  these  tools  and  has  some  other
     command-line parameters, see above.


VARIABLES
     You can set the following environment variables to influence
     the behaviour of GtkLPQ:

     CUPS_SERVER
          The default CUPS-server to connect to

     IPP_PORT
          The default Port of the CUPS-server to connect to

     PRINTER
          The default printer to use

     GTKLP_FORCEX
          Set to 1 to force X-Mode for lpr and lp

ERROR
     GtkLPQ produces the follwoing Error-Codes:

     0    normal program termination

     1    small error, wrong parameters, etc.

     2    Server Problems, server not found, no  default  printer
          found, etc.

     3    BAD  error.  Fields  declared  too  short,   too   many
          printers, etc.

SEE ALSO
     gtklp(1), lprm(1), lp(1), CUPS Software Users Manual
     http://localhost:631/documentation.html

COPYRIGHT
     This program was written by T. Mueller, 2000, 2001, 2002.
     It is licensed under the Terms of  the  GNU  GENERAL  PUBLIC
     LICENSE,
     see also http://www.gnu.org.

  
</pre>
