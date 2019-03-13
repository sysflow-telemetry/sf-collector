#include "header.h"

#include "context.h"

void header::writeHeader(Context* cxt) {
   SFHeader header;
   header.version = 1000;
   /*char domain[256];
   memset(domain, 0, 256);
   if(getdomainname(domain, 256) ) {
      cout << "Error calling getdomainname for sysflow header. ERROR: " <<  std::strerror(errno) << endl;
      exit(1); 
   }*/
  /* const scap_machine_info* mi = inspector->get_machine_info();
   if(mi != NULL) {
       header.exporter = mi->hostname;
   }*/
   header.exporter = cxt->exporterID;
   cxt->flow.rec.set_SFHeader(header);
   cxt->numRecs++;
   cxt->dfw->write(cxt->flow);
}
