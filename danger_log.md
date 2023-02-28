# Danger Log

* ### Improvement we made
  
  1. Exception Guarantee - Our proxy has gracefully designed to handle almost all possible errors internally. If exception happens, no side affect would be caused and the proxy would keep working. That is to say, the proxy makes basic guarantee.
  2. Singleton design pattern - Apply singleton pattern to cache and log class to ensure that resources are used efficiently by preventing the creation of unnecessary instances of them.
  3. Boost library - A very powerful library that can handle many work for us. We utilize the library to make our proxy more robust and more effective.
* ### Further Improvement
  
  1. User Input - The proxy accepts user input, such as URLs or request headers, but such input might need to be sanitized before using it in a request.
  2. Request Method - Any http request methods except CONNECT, GET and POST is beyond the competence of this proxy.
  3. Response status code - Feature of handling many other status code needs to be updated for the proxy.
  4. Security - The proxy may be vulnerable to various attacks due to the fact that it lacks protection policies for security.
  5. We only obey some caching rules, not all of them in RFC7234, so our proxy can only handle some common situations.
  6. Thread-safe - We use read/write locks in our cache, which is more efficient than locking the whole data structure, but maybe we are far from a real world thread safe solution.
  7. Concurrency - To implement multi-thread, we use pthread to handle each incoming connection, but a thread pool can be a better choice.
  8. Massive request - We only tested with a few http request at the same time, so it is very likely that our proxy would crash when handling hundreds of requests simultaneously.