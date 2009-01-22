%% Copyright (c) 2008 Nick Gerakines <nick@gerakines.net>
%% 
%% Permission is hereby granted, free of charge, to any person
%% obtaining a copy of this software and associated documentation
%% files (the "Software"), to deal in the Software without
%% restriction, including without limitation the rights to use,
%% copy, modify, merge, publish, distribute, sublicense, and/or sell
%% copies of the Software, and to permit persons to whom the
%% Software is furnished to do so, subject to the following
%% conditions:
%% 
%% The above copyright notice and this permission notice shall be
%% included in all copies or substantial portions of the Software.
%% 
%% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
%% EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
%% OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
%% NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
%% HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
%% WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
%% FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
%% OTHER DEALINGS IN THE SOFTWARE.
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
