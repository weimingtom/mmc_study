
#include "apr_memcache.h"
#define PORT 11211
#define HOST "localhost"

#define out(funa) fprintf(stdout, funa " : %d %s\n", rv, apr_strerror(rv, buf, sizeof buf));
int main( int argc, char**argv ) 
{
    apr_pool_t *p;
    char buf[120];
    apr_status_t rv;
    apr_memcache_t *memcache;
    apr_memcache_server_t *server;
    apr_memcache_stats_t* stats;
    apr_size_t len;
    char *result;
    apr_uint32_t new;
    apr_uint32_t until = 600;

    apr_initialize();
    atexit(apr_terminate);
    apr_pool_create(&p, NULL);

    rv = apr_memcache_create(p, 10, 0, &memcache);
    out("_create")

    rv = apr_memcache_server_create(p, HOST, PORT, 0, 1, 1, 60, &server);
    out("_create_server")

    rv = apr_memcache_add_server(memcache, server);
    out("_add_server")
    
	result = "";
    rv = apr_memcache_version(server, p, &result);
    out("_server_version")
    printf("\tresult: '%s'\n", result);

	stats = 0;
    rv = apr_memcache_stats(server, p, &stats);
    out("_server_stats")
	if(stats != 0)
		printf("\tpid '%d' version: '%s'\n", stats->pid, stats->version);
	else
		printf("\tapr_memcache_stats() faild");

    rv = apr_memcache_set(memcache, "foo", "bar123", sizeof("bar123")-1, until, 0);
    out("_set")

	result = "";
    rv = apr_memcache_getp(memcache, p, "foo", &result, &len, NULL);
    out("_get")
    printf("\tresult: '%s' len: %d\n", result, len);

    rv = apr_memcache_delete(memcache, "foo", 100);
    out("_delete")

    /* the next three should fail, since foo was just deleted */
    result = "";
	rv = apr_memcache_getp(memcache, p, "foo", &result, &len, NULL);
    out("_get_empty")
  
    rv = apr_memcache_replace(memcache, "foo", "bar123", sizeof("bar123")-1, until, 0);
    out("_replace_non_exist")

	result = "";
    rv = apr_memcache_getp(memcache, p, "foo", &result, &len, NULL);
    out("_get_was_not_replaced")

    rv = apr_memcache_set(memcache, "foo", "1", sizeof("1")-1, until, 0);
    out("_set")

	result = "";
    rv = apr_memcache_getp(memcache, p, "foo", &result, &len, NULL);
    out("_get")
    printf("\tresult: '%s' len: %d\n", result, len);
    
    rv = apr_memcache_incr(memcache, "foo", 5, NULL);
    out("_incr")

	result = "";
    rv = apr_memcache_getp(memcache, p, "foo", &result, &len, NULL);
    out("_get")
    printf("\tresult: '%s' len: %d\n", result, len);
    
    rv = apr_memcache_decr(memcache, "foo", 2, NULL);
    out("_decr")

	result = "";
    rv = apr_memcache_getp(memcache, p, "foo", &result, &len, NULL);
    out("_get")
    printf("\tresult: '%s' len: %d\n", result, len);

    rv = apr_memcache_incr(memcache, "foo", -2, &new);
    out("_incr")

	result = "";
    rv = apr_memcache_getp(memcache, p, "foo", &result, &len, NULL);
    out("_get")
    printf("\tresult: '%s'='%d' len: %d\n", result, new, len);
    
    return rv;
}

