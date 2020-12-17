Assignment 2: smallsh
Name: Hao Jia


os1 ~/CS344/assignemnt-2 139% gcc -o smallsh smallsh.c
os1 ~/CS344/assignemnt-2 140% smallsh
: ls
1  junk  junk2	p2testscript  README.md  smallsh  smallsh.c
: ls > junk
: status
exit value 0
: cat junk     
1
junk
junk2
p2testscript
README.md
smallsh
smallsh.c
: wc < junk > junk2
: wc < junk
 7  7 54
: test -f badfile
: status
exit value 1
: badfile
execvp: No such file or directory
: sleep 5
: sleep 5
^Cterminated by signal 2
: status &
exit value 1
: sleep 15 &
This is a background: 96399
: ps
   PID TTY          TIME CMD
 19243 pts/69   00:00:00 tcsh
 30344 pts/69   00:00:00 smallsh
 96399 pts/69   00:00:00 sleep
 97009 pts/69   00:00:00 ps
:
: # that was a blank command line, this is a comment line
Background 96399 terminated.
Child 96399 exited normally with status 0
: #the background sleep finally finished
: sleep 30 &
This is a background: 112855
: kill -15 112855
: pwd
/nfs/stak/users/jiaha/CS344/assignemnt-2
Background 112855 terminated.
Child 112855 exited abnormally due to signal 15
: cd
: pwd
/nfs/stak/users/jiaha
: echo 4867
4867
: echo $$
30344
: ^C^ZEntering foreground-only mode
(& is ignored)
 : date
Tue Jul 21 21:34:58 PDT 2020
: sleep 5 &
This is a background: 133073
: date
Tue Jul 21 21:35:14 PDT 2020
Background 133073 terminated.
Child 133073 exited normally with status 0
: sleep 5 &
This is a background: 138674
: date
Tue Jul 21 21:35:34 PDT 2020
: exit
