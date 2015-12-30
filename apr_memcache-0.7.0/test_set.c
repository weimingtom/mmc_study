
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
    apr_size_t len;
    char *result;
    int i = 100000;
    apr_time_t start, end;
    double res;
    apr_short_interval_time_t until =  apr_time_make(1000,0);

    apr_initialize();
    atexit(apr_terminate);
    apr_pool_create(&p, NULL);

    rv = apr_memcache_create(p, 10, 0, &memcache);
    out("_create")

    rv = apr_memcache_server_create(p, HOST, PORT, 0, 1, 1, 60, &server);
    out("_create_server")

    rv = apr_memcache_add_server(memcache, server);
    out("_add_server")
    
    start = apr_time_now();
    while(i>0) {
        rv = apr_memcache_set(memcache, "foo", "bar123", sizeof("bar123")-1, until, 0);
        if(rv != APR_SUCCESS)
            out("_set")
        i--;
    }
    end = apr_time_now();
    res = apr_time_as_msec((end-start));
    res /= 1000;
    printf("Elapsed: %f\nReq/Sec: %f\n\n", res, ((100000/res)));
    return rv;
}

