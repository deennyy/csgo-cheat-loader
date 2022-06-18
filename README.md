# csgo-cheat-loader

this was written in a couple of days just for fun, i'm not going to pretend it's really secure or anything, just a hobby project <br/>

you will need to download and compile the master branch of BlackBone <br/>
you will need to download and compile version 8.6 of crypto++ <br/>

to use the dll-encryption-tool: <br/>
when you open it, it's gonna ask for a path to a DLL and then output the encrypted DLL in the same folder where Project2.exe is located with the name cheat.dll <br/>
the path needs to have the \ symbol escaped, so for example the path you input would be: C:\\\\Users\\\\denny\\\\Desktop\\\\test.dll <br/>
then just upload it to your server in the /dll directory (the default name is test.dll) <br/>

don't forget to throw an HTTP 403 error when someone accesses the /dll directory in your nginx config or whatever webserver you're using <br/>

you should also probably use VMProtect on both the loader and the dll (before encryption obv) <br/>

credits: <br/>
literally everything is credited in the source itself
