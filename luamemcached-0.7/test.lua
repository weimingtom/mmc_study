require('Memcached')

memcache = Memcached.Connect('127.0.0.1', 11211)
for i=1, 1000 do
memcache:set('some_key', 1234)
memcache:add('new_key', 'add new value')
memcache:replace('existing_key', 'replace old value')

cached_data = memcache:get('some_key')

memcache:delete('old_key')
end

