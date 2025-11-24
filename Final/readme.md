**command to run server** --> 
---> g++ server.cpp dllPool.cpp logic.cpp lru.cpp memoryCache.cpp socketClass.cpp valueStructPool.cpp -o server
---> ./server 5001 78000 20
first argument : port no
second argument : cache memory size in BYTE
third argument : total client at a time allowed to connect to server

**command to run manual client** -->
---> g++ client.cpp socketClass.cpp -o client
---> ./client

**command to run automated client** -->
---> g++ loader.cpp socketClass.cpp -o loader
---> ./loader

**SOME debugger tool use**
1. AddressSanitizer (ASan) 
2. lldb debugger

**DESCRIBTION**
Project: Multithreaded Key-Value Store Server with LRU Caching and PostgreSQL Persistence

A high-performance, multithreaded key-value store server designed to handle concurrent client requests efficiently. The system supports in-memory caching using an LRU (Least Recently Used)(WITH MEMORY POOL) policy for fast data retrieval and integrates with PostgreSQL to ensure persistent data storage. It features multithreaded HTTP APIs that enable concurrent ADD, READ, UPDATE, and DELETE operations, ensuring scalability and responsiveness under heavy workloads.

Key Highlights:
1. Multithreading: Efficiently handles multiple client connections simultaneously using POSIX threads.
2. LRU Cache: Implements an optimized in-memory cache for rapid access to frequently used data.
3. Persistence Layer: Synchronizes cached data with PostgreSQL for durable storage and recovery.
4. Performance Focused: Designed for low-latency access, optimized memory usage, and high throughput under concurrent load.


**KeyFeature**
1. Use a Cache Memory (in RAM) (SIZE CUSTAMIZABLE)to implement a IO bound operation
2. Use a memoryPool for cache memory to improve the performance of System 
3. use LRU policy to add and remove item in cache 
4. use a double linked node memory pool to make LRU efficient 
5. use POSTGRES SQL to prove a (DISC HEAVY) non-volatile memory
6. use lock and pthread to councerrency and Syncronization 


**IMPORTANT**
--->first character of key will a from 'a' to 'z'

**INPUT COMMAND FORMAT**
1. add a key value in (server)db -> [ADD] [YOUR_KEY] [KEY_VALUE]
2. update a key's value in (server)db -> [UPDATE] [YOUR_KEY] [KEY_VALUE]
3. delete a key -> [DELETE] [YOUR_KEY]
4. read a key's value -> [READ] [YOUR_KEY]



PDF LINK --> https://drive.google.com/drive/folders/1IHFEXrDnFeAHzYAyag08CxO2ETpVih7R?usp=sharing

github link --> https://github.com/SuMiT260702/KV-Server