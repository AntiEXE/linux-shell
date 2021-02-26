# ACM-SH Linux Shell

Simple Linux shell written in C adapted from workshop by ACM IIT Roorkee Student Chapter. 

### What Does It Do?

* Executes built-in commands:
    * cd
    * exit
    * bg
    * bglist
    * kill
    * ls
    * pwd
    * help

* Executes external commands by forking child process.

### Built With:
* C
* Ubuntu

### Usage:
Compile the shell by traversing into the directory where the files are stored. In the linux terminal: 
```
$ gcc acmsh.c linkedlist.c linkedlist.h utilities.c utilities.h -o shell 
```
Execute the binary:

```
$ ./shell
```
That's it you are in.

```
>> cd ..
>> ls
>> bglist
```

### Acknowledgments
* ACM IIT Roorkee Student Chapter
* https://brennan.io/2015/01/16/write-a-shell-in-c/
