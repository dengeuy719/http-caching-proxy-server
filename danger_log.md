# Danger Log
* Improvement that made
  1. Exception Guarantee - Our proxy has gracefully designed to handle all possible errors internally, without throwing any exceptions. That is to say, the proxy makes No-throw guarantee.
  2. Singleton design pattern - Apply singleton pattern to cache and log class to ensure that resources are used efficiently by preventing the creation of unnecessary instances of them.
* Further Improvement
  1. User Input - The proxy accepts user input, such as URLs or request headers, but such input might need to be sanitized before using it in a request.
  2. Request Method - Any http request methods except CONNECT, GET and POST is beyond the competence of this proxy.
  3. Response status code - Feature of handling many other status code needs to be updated for the proxy.
  4. Security - The proxy may be vulnerable to various attacks due to the fact that it lacks protection policies for security.
    