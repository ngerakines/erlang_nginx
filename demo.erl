%% @doc An erlang_nginx request handler
-module(demo).
-export([start/0, server/0]).

%% @doc Starts the server process
start() ->
    Pid = spawn(?MODULE, server, []),
    register(demo_handler, Pid),
    Pid.

%% @private
server() ->
    receive
        {Pid, Method, Uri} -> handler_request(Pid, Method, Uri);
        X -> ok
    end,
    demo:server().

%% @spec handle_request(Pid, Method, Uri) -> ok
%%       Pid = process()
%%       Method = integer()
%%       Uri = string()
%% @doc Process requests from nginx.
handler_request(Pid, Method, Uri) ->
    io:format("{Pid, Method, Uri}: ~p~n", [{Pid, Method, Uri}]),
    Pid ! {200, <<"This is a really cool test.">>},
    ok.
