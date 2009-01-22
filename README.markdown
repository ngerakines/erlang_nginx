This is an **experimental** erlang extension for nginx.

Contributions are welcome. I don't claim to write beautiful c. The biggest thing you can do to help is to show me what I did wrong (nicely) so I can learn from it.

## Build

From the nginx source directory, configure nginx with the module included.

    $ ./configure --add-module=/path/to/erlang_nginx
    $ make
    $ make install

Meanwhile

    $ erl -setcookie secret -sname handler@localhost
    1> P = demo:start().
    <x.x.x>

Running nginx and making a curl request ala

    $ curl http://localhost/foobar
    This...

Results in this output in Erlang:

    {Pid, Method, Uri}: {<8207.9.0>, 2, "/foo" ++ _}

## Configuration

    location /app/ {
        erlang;
    }

## TODO

 * ngx\_http\_erlang\_module.c (66) - Get this from nginx.
 * ngx\_http\_erlang\_module.c (69) - Set this during the erlang message recieve loop.
 * ngx\_http\_erlang\_module.c (82) - Add secret to config
 * ngx\_http\_erlang\_module.c (88) - Add node name to config
 * ngx\_http\_erlang\_module.c (89) - Add node host to config
 * ngx\_http\_erlang\_module.c (97) - Send the headers and request body
 * ngx\_http\_erlang\_module.c (103) - Set the registered process that is the handler in the config.
 * ngx\_http\_erlang\_module.c (106) - Find a way to fail gracefully if a message isn't recieved within
 * ngx\_http\_erlang\_module.c (108) - Set the timeout duration in config.
 * ngx\_http\_erlang\_module.c (120) - Handle response tuple: {StatusCode, Headers, Body}
 * ngx\_http\_erlang\_module.c (126) - Set status code from response tuple

## License

This software is Open Source under the MIT license.
