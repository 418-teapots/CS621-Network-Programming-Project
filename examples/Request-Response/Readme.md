## HOWTO make and use a new application

1. copy 'request-reponse-client.{h,cc}' 'request-reponse-server.{h,cc}' four files into ns-3.29/src/applications/model

copy 'request-reponse-helper.{h, cc}' two files into ns-3.29/src/applications/helper
2. if src/applications/wscript has never change before, replace it with 'wscript';
replace ns-3.29/examples/udp/wscript with wscript-udp (remember to rename the file 'wscript-udp' to 'wscript')
copy 'request-response.cc' into 'ns-3.29/examples/udp/'
else add ```
        'model/request-response-client.h',
        'model/request-response-server.h',
``` in ```headers.source```
add ```
        'model/request-response-client.cc',
        'model/request-response-server.cc',
``` in ```module.source```


3. ```cd ns-3.29
./ waf build
./ waf
```
4. Run application
```
./waf --run request-response
```
