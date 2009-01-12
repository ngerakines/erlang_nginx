-module(demo).
-export([start/0, server/0]).

start() ->
    Pid = spawn(?MODULE, server, []),
    register(demo_handler, Pid),
    Pid.

server() ->
    receive
        {Pid, Method, Uri} ->
            io:format("{Pid, Method, Uri}: ~p~n", [{Pid, Method, Uri}]),
            Pid ! {200, <<"This is a really cool test.">>};
        X -> io:format("X: ~p~n", [X])
    end,
    demo:server().
