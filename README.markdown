This is an **experimental** erlang extension for nginx.

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

 * Parsing of request headers and body into the Erlang handler
 * Taking the status, headers and body from the handler and doing stuff with it.

Contributions are welcome. I don't claim to write beautiful c. The biggest thing you can do to help is to show me what I did wrong (nicely) so I can learn from it.
