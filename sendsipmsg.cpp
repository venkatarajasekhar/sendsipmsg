
#include "rutil/Log.hpp"
#include "rutil/Logger.hpp"
#include "rutil/Subsystem.hpp"
#include "resip/dum/ClientAuthManager.hpp"
#include "resip/dum/ClientRegistration.hpp"
#include "resip/dum/DialogUsageManager.hpp"
#include "resip/dum/InviteSessionHandler.hpp"
#include "resip/dum/MasterProfile.hpp"
#include "resip/dum/Profile.hpp"
#include "resip/dum/UserProfile.hpp"
#include "resip/dum/RegistrationHandler.hpp"
#include "resip/dum/ClientPagerMessage.hpp"
#include "resip/dum/ServerPagerMessage.hpp"

#include "resip/dum/DialogUsageManager.hpp"
#include "resip/dum/AppDialogSet.hpp"
#include "resip/dum/AppDialog.hpp"
#include "resip/dum/RegistrationHandler.hpp"
#include "resip/dum/PagerMessageHandler.hpp"
#include "resip/stack/PlainContents.hpp"

#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace resip;

#define RESIPROCATE_SUBSYSTEM Subsystem::TEST

class ClientMessageHandler : public ClientPagerMessageHandler {
public:
   ClientMessageHandler()
      : try{
         finished(false),
        successful(false)
      }catch(...){
         
      }
   {
   };

   //virtual void onSuccess(ClientPagerMessageHandle, const SipMessage& status)
   virtual void onSuccess(ClientPagerMessageHandle &CliPagerMsgHandleObj, const SipMessage& status)
   {
      //Logic needs to be implement
      
      InfoLog(<<"ClientMessageHandler::onSuccess\n");
      successful = true;
      finished = true;
   }

   //virtual void onFailure(ClientPagerMessageHandle, const SipMessage& status, std::auto_ptr<Contents> contents)
   virtual void onFailure(ClientPagerMessageHandle &CliPagerMsgHandleObj , const SipMessage& status, auto_ptr<Contents> contents)
   {
      //Logic needs to be implement
      ErrLog(<<"ClientMessageHandler::onFailure\n");
      successful = false;
      finished = true;
   }

   bool isFinished() { return finished; };
   bool isSuccessful() { return successful; };

private:
   bool finished;
   bool successful;
};

int main(int argc, char *argv[])
{
   Log::initialize(Log::Cout, Log::Info, argv[0]);

   if( (argc < 6) || (argc > 7) ) {
      ErrLog(<< "usage: " << argv[0] << " sip:from user passwd realm sip:to [port]\n");
      return 1;
   }
   try{
   string from(argv[1]);
   }catch(...){
      
   }
   try{
   string user(argv[2]);
   }catch(...){
      
   }
   try{
   string passwd(argv[3]);
   }catch(...){
      
   }
   try{
   string realm(argv[4]);
   }catch(...){
      
   }
   try{
      string to(argv[5]);
   }catch(...){
      
   }
   int port = 5060;
   if(argc == 7)
   {
      string temp(argv[6]);
      istringstream src(temp);
      src >> port;
   }

   InfoLog(<< "log: from: " << from << ", to: " << to << ", port: " << port << "\n");
   InfoLog(<< "user: " << user << ", passwd: " << passwd << ", realm: " << realm << "\n");
   try{ 
   <MasterProfile> profile(new MasterProfile);
   }catch(...){
      
   }
   try{
      <ClientAuthManager> clientAuth(new ClientAuthManager());
   }catch(...){
      
   }
   try{
   SipStack clientStack;
   }catch(...){
      
   }
   try{
   DialogUsageManager clientDum(clientStack);
   }catch(...){
      
   }
   try{
   clientDum.addTransport(UDP, port);
   }catch(...){
      
   }
   clientDum.setMasterProfile(profile);

   clientDum.setClientAuthManager(clientAuth);
   clientDum.getMasterProfile()->setDefaultRegistrationTime(70);
   clientDum.getMasterProfile()->addSupportedMethod(MESSAGE);
   clientDum.getMasterProfile()->addSupportedMimeType(MESSAGE, Mime("text", "plain"));
   try{
   ClientMessageHandler *cmh = new ClientMessageHandler();
   }catch(...){
      
   }
   clientDum.setClientPagerMessageHandler(cmh);

   NameAddr naFrom(from.c_str());
   profile->setDefaultFrom(naFrom);
   profile->setDigestCredential(realm.c_str(), user.c_str(), passwd.c_str());

   InfoLog(<< "Sending MESSAGE\n");
   try{
   NameAddr naTo(to.c_str());
   }catch(...){
      
   }
   ClientPagerMessageHandle cpmh;   //ctor
   clientDum.makePagerMessage(naTo);
   try{
   Data messageBody("Hello world!");  //ctor
   }catch(...){
      
   }
   <Contents> content(new PlainContents(messageBody));
   cpmh.get()->page(content);

   // Event loop - stack will invoke callbacks in our app
   while(!cmh->isFinished())
   {
      try{
      clientStack.process(100);
      }catch(...){
         
      }
      while(clientDum.process());
   }

   if(!cmh->isSuccessful())
   {
      ErrLog(<< "Message delivery failed, aborting");
      return 1;
   }

   return 0;
}

