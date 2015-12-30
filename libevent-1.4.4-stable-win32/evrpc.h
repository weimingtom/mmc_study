/*
 * Copyright (c) 2006 Niels Provos <provos@citi.umich.edu>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _EVRPC_H_
#define _EVRPC_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file evrpc.h
 *
 * This header files provides basic support for an RPC server and client.
 *
 * To support RPCs in a server, every supported RPC command needs to be
 * defined and registered.
 *
 * EVRPC_HEADER(SendCommand, Request, Reply);
 *
 *  SendCommand is the name of the RPC command.
 *  Request is the name of a structure generated by event_rpcgen.py.
 *    It contains all parameters relating to the SendCommand RPC.  The
 *    server needs to fill in the Reply structure.
 *  Reply is the name of a structure generated by event_rpcgen.py.  It
 *    contains the answer to the RPC.
 *
 * To register an RPC with an HTTP server, you need to first create an RPC
 * base with:
 *
 *   struct evrpc_base *base = evrpc_init(http);
 *
 * A specific RPC can then be registered with
 *
 * EVRPC_REGISTER(base, SendCommand, Request, Reply,  FunctionCB, arg);
 *
 * when the server receives an appropriately formatted RPC, the user callback
 * is invokved.   The callback needs to fill in the reply structure.
 *
 * void FunctionCB(EVRPC_STRUCT(SendCommand)* rpc, void *arg);
 *
 * To send the reply, call EVRPC_REQUEST_DONE(rpc);
 *
 * See the regression test for an example.
 */

struct evbuffer;
struct event_base;
struct evrpc_req_generic;

/* Encapsulates a request */
struct evrpc {
	TAILQ_ENTRY(evrpc) next;

	/* the URI at which the request handler lives */
	const char* uri;

	/* creates a new request structure */
	void *(*request_new)(void);

	/* frees the request structure */
	void (*request_free)(void *);

	/* unmarshals the buffer into the proper request structure */
	int (*request_unmarshal)(void *, struct evbuffer *);

	/* creates a new reply structure */
	void *(*reply_new)(void);

	/* creates a new reply structure */
	void (*reply_free)(void *);

	/* verifies that the reply is valid */
	int (*reply_complete)(void *);
	
	/* marshals the reply into a buffer */
	void (*reply_marshal)(struct evbuffer*, void *);

	/* the callback invoked for each received rpc */
	void (*cb)(struct evrpc_req_generic *, void *);
	void *cb_arg;

	/* reference for further configuration */
	struct evrpc_base *base;
};

/** The type of a specific RPC Message
 *
 * @param rpcname the name of the RPC message
 */
#define EVRPC_STRUCT(rpcname) struct evrpc_req__##rpcname

struct evhttp_request;
struct evrpc_status;

/* We alias the RPC specific structs to this voided one */
struct evrpc_req_generic {
	/* the unmarshaled request object */
	void *request;

	/* the empty reply object that needs to be filled in */
	void *reply;

	/* 
	 * the static structure for this rpc; that can be used to
	 * automatically unmarshal and marshal the http buffers.
	 */
	struct evrpc *rpc;

	/*
	 * the http request structure on which we need to answer.
	 */
	struct evhttp_request* http_req;

	/*
	 * callback to reply and finish answering this rpc
	 */
	void (*done)(struct evrpc_req_generic* rpc); 
};

/** Creates the definitions and prototypes for an RPC
 *
 * You need to use EVRPC_HEADER to create structures and function prototypes
 * needed by the server and client implementation.  The structures have to be
 * defined in an .rpc file and converted to source code via event_rpcgen.py
 *
 * @param rpcname the name of the RPC
 * @param reqstruct the name of the RPC request structure
 * @param replystruct the name of the RPC reply structure
 * @see EVRPC_GENERATE()
 */
#define EVRPC_HEADER(rpcname, reqstruct, rplystruct) \
EVRPC_STRUCT(rpcname) {	\
	struct reqstruct* request; \
	struct rplystruct* reply; \
	struct evrpc* rpc; \
	struct evhttp_request* http_req; \
	void (*done)(struct evrpc_status *, \
	    struct evrpc* rpc, void *request, void *reply);	     \
};								     \
int evrpc_send_request_##rpcname(struct evrpc_pool *, \
    struct reqstruct *, struct rplystruct *, \
    void (*)(struct evrpc_status *, \
	struct reqstruct *, struct rplystruct *, void *cbarg),	\
    void *);

/** Generates the code for receiving and sending an RPC message
 *
 * EVRPC_GENERATE is used to create the code corresponding to sending
 * and receiving a particular RPC message
 *
 * @param rpcname the name of the RPC
 * @param reqstruct the name of the RPC request structure
 * @param replystruct the name of the RPC reply structure
 * @see EVRPC_HEADER()
 */
#define EVRPC_GENERATE(rpcname, reqstruct, rplystruct) \
int evrpc_send_request_##rpcname(struct evrpc_pool *pool, \
    struct reqstruct *request, struct rplystruct *reply, \
    void (*cb)(struct evrpc_status *, \
	struct reqstruct *, struct rplystruct *, void *cbarg),	\
    void *cbarg) { \
	struct evrpc_status status;				    \
	struct evrpc_request_wrapper *ctx;			    \
	ctx = (struct evrpc_request_wrapper *) \
	    malloc(sizeof(struct evrpc_request_wrapper));	    \
	if (ctx == NULL)					    \
		goto error;					    \
	ctx->pool = pool;					    \
	ctx->evcon = NULL;					    \
	ctx->name = strdup(#rpcname);				    \
	if (ctx->name == NULL) {				    \
		free(ctx);					    \
		goto error;					    \
	}							    \
	ctx->cb = (void (*)(struct evrpc_status *, \
		void *, void *, void *))cb;			    \
	ctx->cb_arg = cbarg;					    \
	ctx->request = (void *)request;				    \
	ctx->reply = (void *)reply;				    \
	ctx->request_marshal = (void (*)(struct evbuffer *, void *))reqstruct##_marshal; \
	ctx->reply_clear = (void (*)(void *))rplystruct##_clear;    \
	ctx->reply_unmarshal = (int (*)(void *, struct evbuffer *))rplystruct##_unmarshal; \
	return (evrpc_make_request(ctx));			    \
error:								    \
	memset(&status, 0, sizeof(status));			    \
	status.error = EVRPC_STATUS_ERR_UNSTARTED;		    \
	(*(cb))(&status, request, reply, cbarg);		    \
	return (-1);						    \
}

/** Provides access to the HTTP request object underlying an RPC
 *
 * Access to the underlying http object; can be used to look at headers or
 * for getting the remote ip address
 *
 * @param rpc_req the rpc request structure provided to the server callback
 * @return an struct evhttp_request object that can be inspected for
 * HTTP headers or sender information.
 */
#define EVRPC_REQUEST_HTTP(rpc_req) (rpc_req)->http_req

/** Creates the reply to an RPC request
 * 
 * EVRPC_REQUEST_DONE is used to answer a request; the reply is expected
 * to have been filled in.  The request and reply pointers become invalid
 * after this call has finished.
 * 
 * @param rpc_req the rpc request structure provided to the server callback
 */
#define EVRPC_REQUEST_DONE(rpc_req) do { \
  struct evrpc_req_generic *_req = (struct evrpc_req_generic *)(rpc_req); \
  _req->done(_req); \
} while (0)
  

/* Takes a request object and fills it in with the right magic */
#define EVRPC_REGISTER_OBJECT(rpc, name, request, reply) \
  do { \
    (rpc)->uri = strdup(#name); \
    if ((rpc)->uri == NULL) {			 \
      fprintf(stderr, "failed to register object\n");	\
      exit(1);						\
    } \
    (rpc)->request_new = (void *(*)(void))request##_new; \
    (rpc)->request_free = (void (*)(void *))request##_free; \
    (rpc)->request_unmarshal = (int (*)(void *, struct evbuffer *))request##_unmarshal; \
    (rpc)->reply_new = (void *(*)(void))reply##_new; \
    (rpc)->reply_free = (void (*)(void *))reply##_free; \
    (rpc)->reply_complete = (int (*)(void *))reply##_complete; \
    (rpc)->reply_marshal = (void (*)(struct evbuffer*, void *))reply##_marshal; \
  } while (0)

struct evrpc_base;
struct evhttp;

/* functions to start up the rpc system */

/** Creates a new rpc base from which RPC requests can be received
 *
 * @param server a pointer to an existing HTTP server
 * @return a newly allocated evrpc_base struct
 * @see evrpc_free()
 */
struct evrpc_base *evrpc_init(struct evhttp *server);

/** 
 * Frees the evrpc base
 *
 * For now, you are responsible for making sure that no rpcs are ongoing.
 *
 * @param base the evrpc_base object to be freed
 * @see evrpc_init
 */
void evrpc_free(struct evrpc_base *base);

/** register RPCs with the HTTP Server
 *
 * registers a new RPC with the HTTP server, each RPC needs to have
 * a unique name under which it can be identified.
 *
 * @param base the evrpc_base structure in which the RPC should be
 *   registered.
 * @param name the name of the RPC
 * @param request the name of the RPC request structure
 * @param reply the name of the RPC reply structure
 * @param callback the callback that should be invoked when the RPC
 * is received.  The callback has the following prototype
 *   void (*callback)(EVRPC_STRUCT(Message)* rpc, void *arg)
 * @param cbarg an additional parameter that can be passed to the callback.
 *   The parameter can be used to carry around state.
 */
#define EVRPC_REGISTER(base, name, request, reply, callback, cbarg) \
  do { \
    struct evrpc* rpc = (struct evrpc *)calloc(1, sizeof(struct evrpc)); \
    EVRPC_REGISTER_OBJECT(rpc, name, request, reply); \
    evrpc_register_rpc(base, rpc, \
	(void (*)(struct evrpc_req_generic*, void *))callback, cbarg);	\
  } while (0)

int evrpc_register_rpc(struct evrpc_base *, struct evrpc *,
    void (*)(struct evrpc_req_generic*, void *), void *);

/**
 * Unregisters an already registered RPC
 *
 * @param base the evrpc_base object from which to unregister an RPC
 * @param name the name of the rpc to unregister
 * @return -1 on error or 0 when successful.
 * @see EVRPC_REGISTER()
 */
#define EVRPC_UNREGISTER(base, name) evrpc_unregister_rpc(base, #name)

int evrpc_unregister_rpc(struct evrpc_base *base, const char *name);

/*
 * Client-side RPC support
 */

struct evrpc_pool;
struct evhttp_connection;

/** 
 * provides information about the completed RPC request.
 */
struct evrpc_status {
#define EVRPC_STATUS_ERR_NONE		0
#define EVRPC_STATUS_ERR_TIMEOUT	1
#define EVRPC_STATUS_ERR_BADPAYLOAD	2
#define EVRPC_STATUS_ERR_UNSTARTED	3
#define EVRPC_STATUS_ERR_HOOKABORTED	4
	int error;

	/* for looking at headers or other information */
	struct evhttp_request *http_req;
};

struct evrpc_request_wrapper {
	TAILQ_ENTRY(evrpc_request_wrapper) next;

        /* pool on which this rpc request is being made */
        struct evrpc_pool *pool;

        /* connection on which the request is being sent */
	struct evhttp_connection *evcon;

	/* event for implementing request timeouts */
	struct event ev_timeout;

	/* the name of the rpc */
	char *name;

	/* callback */
	void (*cb)(struct evrpc_status*, void *request, void *reply, void *arg);
	void *cb_arg;

	void *request;
	void *reply;

	/* unmarshals the buffer into the proper request structure */
	void (*request_marshal)(struct evbuffer *, void *);

	/* removes all stored state in the reply */
	void (*reply_clear)(void *);

	/* marshals the reply into a buffer */
	int (*reply_unmarshal)(void *, struct evbuffer*);
};

/** launches an RPC and sends it to the server
 *
 * EVRPC_MAKE_REQUEST() is used by the client to send an RPC to the server.
 *
 * @param name the name of the RPC
 * @param pool the evrpc_pool that contains the connection objects over which
 *   the request should be sent.
 * @param request a pointer to the RPC request structure - it contains the
 *   data to be sent to the server.
 * @param reply a pointer to the RPC reply structure.  It is going to be filled
 *   if the request was answered successfully
 * @param cb the callback to invoke when the RPC request has been answered
 * @param cbarg an additional argument to be passed to the client
 * @return 0 on success, -1 on failure
 */
#define EVRPC_MAKE_REQUEST(name, pool, request, reply, cb, cbarg)	\
	evrpc_send_request_##name(pool, request, reply, cb, cbarg)

int evrpc_make_request(struct evrpc_request_wrapper *);

/** creates an rpc connection pool
 * 
 * a pool has a number of connections associated with it.
 * rpc requests are always made via a pool.
 *
 * @param base a pointer to an struct event_based object; can be left NULL
 *   in singled-threaded applications
 * @return a newly allocated struct evrpc_pool object
 * @see evrpc_pool_free()
 */
struct evrpc_pool *evrpc_pool_new(struct event_base *base);
/** frees an rpc connection pool
 *
 * @param pool a pointer to an evrpc_pool allocated via evrpc_pool_new()
 * @see evrpc_pool_new()
 */
void evrpc_pool_free(struct evrpc_pool *pool);
/*
 * adds a connection over which rpc can be dispatched.  the connection
 * object must have been newly created.
 */
void evrpc_pool_add_connection(struct evrpc_pool *, 
    struct evhttp_connection *);

/**
 * Sets the timeout in secs after which a request has to complete.  The
 * RPC is completely aborted if it does not complete by then.  Setting
 * the timeout to 0 means that it never timeouts and can be used to
 * implement callback type RPCs.
 *
 * Any connection already in the pool will be updated with the new
 * timeout.  Connections added to the pool after set_timeout has be
 * called receive the pool timeout only if no timeout has been set
 * for the connection itself.
 *
 * @param pool a pointer to a struct evrpc_pool object
 * @param timeout_in_secs the number of seconds after which a request should
 *   timeout and a failure be returned to the callback.
 */
void evrpc_pool_set_timeout(struct evrpc_pool *pool, int timeout_in_secs);

/**
 * Hooks for changing the input and output of RPCs; this can be used to
 * implement compression, authentication, encryption, ...
 */

enum EVRPC_HOOK_TYPE {
	EVRPC_HOOK_INPUT,		/**< apply the function to an input hook */
	EVRPC_HOOK_OUTPUT		/**< apply the function to an output hook */
};

/** adds a processing hook to either an rpc base or rpc pool
 *
 * If a hook returns -1, the processing is aborted.
 *
 * The add functions return handles that can be used for removing hooks.
 *
 * @param vbase a pointer to either struct evrpc_base or struct evrpc_pool
 * @param hook_type either EVRPC_HOOK_INPUT or EVRPC_HOOK_OUTPUT
 * @param cb the callback to call when the hook is activated
 * @param cb_arg an additional argument for the callback
 * @return a handle to the hook so it can be removed later
 * @see evrpc_remove_hook()
 */
void *evrpc_add_hook(void *vbase,
    enum EVRPC_HOOK_TYPE hook_type,
    int (*cb)(struct evhttp_request *, struct evbuffer *, void *),
    void *cb_arg);

/** removes a previously added hook
 *
 * @param vbase a pointer to either struct evrpc_base or struct evrpc_pool
 * @param hook_type either EVRPC_HOOK_INPUT or EVRPC_HOOK_OUTPUT
 * @param handle a handle returned by evrpc_add_hook()
 * @return 1 on success or 0 on failure
 * @see evrpc_add_hook()
 */
int evrpc_remove_hook(void *vbase,
    enum EVRPC_HOOK_TYPE hook_type,
    void *handle);

#ifdef __cplusplus
}
#endif

#endif /* _EVRPC_H_ */
