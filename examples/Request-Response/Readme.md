## HOWTO make and use a new application

1. Copy file to the corresponding path
   1. **model**: copy ```request-reponse-client.{h,cc}``` ```request-reponse-server.{h,cc}```  four files into ```ns-3.29/src/applications/model```
   2. **helper**: copy ```request-reponse-helper.{h, cc}``` two files into ```ns-3.29/src/applications/helper```
   3. **udp/wscript**: if ```ns-3.29/examples/udp/wscript``` has never change before, replace it with ```wscript-udp``` (remember to rename the file 'wscript-udp' to 'wscript')
      copy ```request-response.cc``` into ```ns-3.29/examples/udp/```

   4. **wscript**: if ```src/applications/wscript``` has never change before, replace it with ```wscript```;


3. cd ```ns-3.29/``` and build the whole things

   ```./ waf build``` 

   ```./ waf```

4. Run application

```./waf --run request-response```

5. Observe new ASCII trace and pcap trace files in the top-level directory(Use first.cc)