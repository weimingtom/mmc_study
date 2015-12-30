
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
    
    rv = apr_memcache_stats(server, p, &stats);
    out("_server_stats")

    printf("version: '%s'\n", stats->version);
    printf("pid \t'%d'\n", stats->pid);
    printf("uptime: \t'%d'\n", stats->uptime);
    printf("time: \t'%" APR_INT64_T_FMT "'\n", stats->time);
    printf("rusage_user: \t'%" APR_INT64_T_FMT "'\n", stats->rusage_user);
    printf("rusage_system: \t'%" APR_INT64_T_FMT "'\n", stats->rusage_system);
    printf("curr_items: \t'%d'\n", stats->curr_items);
    printf("total_items: \t'%d'\n", stats->total_items);
    printf("bytes: \t'%" APR_UINT64_T_FMT "'\n", stats->bytes);
    printf("curr_connections: \t'%d'\n", stats->curr_connections);
    printf("total_connections: \t'%d'\n", stats->total_connections);
    printf("connection_structures: \t'%d'\n", stats->connection_structures);
    printf("cmd_get: \t'%d'\n", stats->cmd_get);
    printf("cmd_set: \t'%d'\n", stats->cmd_set);
    printf("get_hits: \t'%d'\n", stats->get_hits);
    printf("get_misses: \t'%d'\n", stats->get_misses);
    printf("bytes_read: \t'%" APR_UINT64_T_FMT "'\n", stats->bytes_read);
    printf("bytes_written: \t'%" APR_UINT64_T_FMT "'\n", stats->bytes_written);
    printf("limit_maxbytes: \t'%d'\n", stats->limit_maxbytes);

    return rv;
}

