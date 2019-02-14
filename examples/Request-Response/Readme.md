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

   $ tcpdump -nn -tt -r request-response-0-1.pcap
   
   reading from file request-response-0-1.pcap, link-type EN10MB (Ethernet)
   2.008000 ARP, Request who-has 10.1.1.2 (ff:ff:ff:ff:ff:ff) tell 10.1.1.1, length 50
   2.012205 ARP, Reply 10.1.1.2 is-at 00:00:00:00:00:02, length 50
   2.012205 IP 10.1.1.1.49153 > 10.1.1.2.9: UDP, length 10
   2.017411 ARP, Request who-has 10.1.1.1 (ff:ff:ff:ff:ff:ff) tell 10.1.1.2, length 50
   2.017411 ARP, Reply 10.1.1.1 is-at 00:00:00:00:00:01, length 50
   2.023227 IP 10.1.1.2.9 > 10.1.1.1.49153: UDP, length 1024



