# nolib-server

A simple C sever that can serve files. Has a defualt redirect to 404.html.

There are no import statements and it should be statically compiled without stdlib. 
```gcc server.c -nostdlib -fno-stack-protector -o server -static```
