# LIL'HTTP: A Little HTTP Server #

## What Is It? ##

Lil'Http is a small web-server written in C.  It currently supports get requests.  It is a single threaded server that handles requests asynchrously using epoll, the linux event notification facility.  Benchmarks will hopefully be forthcoming.

## How To Use It ##
To build the server, cd into the src directory and use *make* with no arguments.After compilation, the server will be found in the bin directory.  To run the server, invoke with one argument--the portnumber to be used.  For example

		./server 8000

will run the server from port 8000.  To remove the compiled file and other object files made during compilation, use *make* *clean*

## To Do ##
Currently, the server can only handle requests for files in the same directory it sits in--to remedy that a router should be written.
