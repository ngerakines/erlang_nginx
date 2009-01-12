
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "erl_interface.h"
#include "ei.h"

#define BUFSIZE 1000

extern const char *erl_thisnodename(void); 
extern short erl_thiscreation(void); 
#define SELF(fd) erl_mk_pid(erl_thisnodename(),fd,0,erl_thiscreation())

static char *ngx_http_erlang(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t  ngx_http_erlang_commands[] = {
    {
        ngx_string("erlang"),
        NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS,
        ngx_http_erlang,
        0,
        0,
        NULL
    },
    ngx_null_command
};

static ngx_http_module_t  ngx_http_erlang_module_ctx = {
    NULL,                          /* preconfiguration */
    NULL,                          /* postconfiguration */
    NULL,                          /* create main configuration */
    NULL,                          /* init main configuration */
    NULL,                          /* create server configuration */
    NULL,                          /* merge server configuration */
    NULL,                          /* create location configuration */
    NULL                           /* merge location configuration */
};

ngx_module_t  ngx_http_erlang_module = {
    NGX_MODULE_V1,
    &ngx_http_erlang_module_ctx, /* module context */
    ngx_http_erlang_commands,   /* module directives */
    NGX_HTTP_MODULE,               /* module type */
    NULL,                          /* init master */
    NULL,                          /* init module */
    NULL,                          /* init process */
    NULL,                          /* init thread */
    NULL,                          /* exit thread */
    NULL,                          /* exit process */
    NULL,                          /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_int_t ngx_http_erlang_handler(ngx_http_request_t *r) {
    ngx_int_t rc;
    ngx_buf_t *b;
    ngx_chain_t out;

    struct in_addr addr;
    addr.s_addr = inet_addr("192.168.100.94");

    r->headers_out.content_type.len = sizeof("text/plain") - 1;
    r->headers_out.content_type.data = (u_char *) "text/plain";

    int fd;
    int got;
    unsigned char buf[BUFSIZE];
    ErlMessage emsg;
    
    ETERM *status, *retbody;
    
    erl_init(NULL, 0);
    
    if (erl_connect_init(1, "secret", 0) == -1) {
        erl_err_quit("erl_connect_init");
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if ((fd = erl_connect("handler@localhost")) < 0) {
        erl_err_quit("erl_connect");
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    fprintf(stderr, "Connected to handler@localhost\n\r");

    ETERM *arr[3], *emsg2; 
    arr[0] = SELF(fd); 
    arr[1] = erl_mk_int(r->method); 
    arr[2] = erl_mk_string((const char *) r->uri.data);
    emsg2 = erl_mk_tuple(arr, 3); 
    erl_reg_send(fd, "demo_handler", emsg2);

    while (1) {    
        got = erl_receive_msg(fd, buf, BUFSIZE, &emsg);
        fprintf(stderr, "Got message\n\r");
        if (got == ERL_TICK) {
            fprintf(stderr, " -- tick\n\r");
            continue;
        } else if (got == ERL_ERROR) {
            fprintf(stderr, "ERL_ERROR from erl_receive_msg.\n");
            break;
        } else {
            if (emsg.type == ERL_SEND) { // ERL_REG_SEND
                status = erl_element(1, emsg.msg);
                retbody = erl_element(2, emsg.msg);
                int status_code = ERL_INT_VALUE(status);
                char *body = (char *) ERL_BIN_PTR(retbody);
                
                if (status_code == 200) {
                    r->headers_out.status = NGX_HTTP_OK;
                } else {
                    r->headers_out.status = NGX_HTTP_INTERNAL_SERVER_ERROR;
                }
                
                b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
                if (b == NULL) {
                    return NGX_HTTP_INTERNAL_SERVER_ERROR;
                }

                out.buf = b;
                out.next = NULL;

                b->pos = (u_char *) body;
                b->last = (u_char *) body + sizeof(body);
                b->memory = 1;
                b->last_buf = 1;

                r->headers_out.status = NGX_HTTP_OK;
                r->headers_out.content_length_n = sizeof(body);
                r->headers_out.last_modified_time = 23349600;

                rc = ngx_http_send_header(r);
                
                if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
                    return rc;
                }

                erl_free_term(emsg.msg);    
                erl_free_term(status);
                erl_free_term(retbody);
                break;
            }
        }
    }

    return ngx_http_output_filter(r, &out);
}


static char * ngx_http_erlang(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
    ngx_http_core_loc_conf_t  *clcf;
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_erlang_handler;
    return NGX_CONF_OK;
}
