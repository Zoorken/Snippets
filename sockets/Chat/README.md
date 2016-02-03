## Chat Protocol
----

**Client**------------------------------- **Server**

Connect() −−−−−−−−−−−−−−− >

< −−−−−−−−−−−−−−−−−−−−− Hello version

NICK nick −−−−−−−−−−−−−−− >

< −−−−−−−−−−−−−−−−−−−−− OK/ERROR text

MSG text −−−−−−−−−−−−−−− >

< −−−−−−−−−−−−−−−−−−−−− MSG nick text/ERROR text

Nicknames is limited to 12 characters (A-Za-z0-9 ) and messages is limited to
255 characters (any characters except control characters and newlines, utf-8
encoding). Each message is followed by newline (CR).