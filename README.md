# LIL'HTTP: A Little HTTP Server #

## What Is It? ##

Lil'Http is a small web-server written in C.  It currently supports get requests.  It is a single threaded server that handles requests asynchrously using epoll, the linux event notification facility.  Benchmarks will hopefully be forthcoming.

## How To Use It ##
To build the server, cd into the src directory and use *make* with no arguments.During the build process, a directory within *src* called *bin* should be created--the compiled server will be found there.  To run the server, invoke it with one argument--the portnumber to be used.  For example

		./server 8000

will run the server from port 8000.  Once invoked, the server will look for a file named "config.svr" from which it will create its router.  The configuration file should conform to the following simple grammar:

    <request> <path> \n

An example configuration file is included in the src directory.
To remove the compiled file and other object files made during compilation, use *make* *clean*

## To Do ##
Currently, the server can only handle GET requests for static content.  Other request types and dynamic content remain to be implemented.
