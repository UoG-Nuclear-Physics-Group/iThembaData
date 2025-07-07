#include "TTdrDataParser.h"
#include "TTdrDataParserException.h"

#include "TChannel.h"
#include "Globals.h"

#include "TScalerQueue.h"
#include "TTdrMnemonic.h"
#include "TEpicsFrag.h"
#include "TParsingDiagnostics.h"

#include "Rtypes.h"

#include "TFragment.h"
#include "TBadFragment.h"

TTdrDataParser::TTdrDataParser()
   : TDataParser()
{
   fState = EDataParserState::kGood;
}

TTdrDataParser::~TTdrDataParser()
{
}

int TTdrDataParser::Process(std::shared_ptr<TRawEvent> rawEvent)
{
   /// Process this TTdrEvent using the provided data parser.
   /// Returns the total number of fragments read (good and bad).
   // right now the parser only returns the total number of fragments read
   // so we assume (for now) that all fragments are good fragments
   std::shared_ptr<TTdrEvent> event = std::static_pointer_cast<TTdrEvent>(rawEvent);
   return TdrToFragment(event->GetData(), event->GetDataSize());
}

int TTdrDataParser::TdrToFragment(char* data, uint32_t size)
{
   uint64_t* ptr = reinterpret_cast<uint64_t*>(data);

   int                        totalEventsRead   = 0;
   static uint64_t            timeStampHighBits = 0;
   std::shared_ptr<TFragment> eventFrag         = std::make_shared<TFragment>();
   IncrementInputSize(size / 8);   // words of 8 bytes each

   for(size_t i = 0; i < size / 8; ++i) {
      eventFrag->Clear();
      IncrementItemsPopped();
      DecrementInputSize();
      //std::cout<<"0x"<<std::setw(16)<<ptr[i]<<std::endl;
      switch(ptr[i] >> 62) {
      case 0:
         std::cout << "unknown word" << std::endl;
         break;
      case 1:
         std::cout << "trace word" << std::endl;
         break;
      case 2:
         // this word gives us the extended timestamp (and a moduleNumber and channelNumber)
         // the only thing we really care about is the high timestamp bits
         {
            //short moduleNumber = (ptr[i]>>56)&0x3f;
            short infoCode = (ptr[i] >> 52) & 0xf;
            if(((ptr[i] >> 28) & 0xf) != 0x0) {
               std::cout << "not a proper word!" << std::endl;
            }
            //uint64_t timeStamp = ptr[i]&0xfffffff;
            //uint32_t channelNumber = 0;
            switch(infoCode) {
            case 0:
               std::cout << "undefined infoCode " << infoCode << std::endl;
               break;
            case 1:
               //channelNumber = (ptr[i]>>32)&0xfffff;
               break;
            case 2:
            case 3:
            case 4:
            case 7:
               //timeStamp |= ptr[i]&0xfffff00000000>>4;
               timeStampHighBits = (ptr[i] >> 32) & 0xfffff;
               break;
            case 5:
               //timeStamp |= (ptr[i]&0xfffff00000000)<<20;
               timeStampHighBits = (ptr[i] >> 12) & 0xfffff00000;
               break;
            default:
               std::cout << "infoCode " << infoCode << std::endl;
               break;
            }
            //std::cout<<"moduleNumber "<<moduleNumber<<", channelNumber "<<channelNumber<<", timeStamp "<<timeStamp<<std::endl;
            break;
         }
      case 3: {
         // data word with channelNumber, adcData, and 28 low bits of timestamp
         // check if this is the tape-move event
         auto channel = TChannel::GetChannel((ptr[i] >> 48) & 0xfff);
         if(channel != nullptr && static_cast<EDigitizer>(channel->GetDigitizerType()) == EDigitizer::kPixieTapeMove) {
            //PPG
            auto* ppgEvent = new TPPGData;
            switch(channel->GetDetectorNumber()) {
            case 1:
               // tape move for "normal" experiments
               // target finished moving in front of detector for PR294
               ppgEvent->SetNewPPG(EPpgPattern::kTapeMove);
               break;
            case 2:
               // unused for "normal" experiments
               // target moving away from detector for PR294
               ppgEvent->SetNewPPG(EPpgPattern::kBackground);
               break;
            default:
               ppgEvent->SetNewPPG(EPpgPattern::kJunk);
               break;
            }
            ppgEvent->SetLowTimeStamp(ptr[i] & 0xfffffff);
            ppgEvent->SetHighTimeStamp(timeStampHighBits);
            ppgEvent->SetNetworkPacketId(static_cast<Int_t>((ptr[i] >> 32) & 0xffff));
            TPPG::Get()->AddData(ppgEvent);
            continue;
         }
         eventFrag->SetAddress((ptr[i] >> 48) & 0xfff);
         eventFrag->SetTimeStamp((ptr[i] & 0xfffffff) | (timeStampHighBits << 28));
         ++totalEventsRead;
         // charge is a 14bit signed integer (despite being reported as 16 bits) so we extend the sign bit for an Int_t (4 bytes)
         //eventFrag->SetCharge(static_cast<Int_t>(((ptr[i]>>32)&0xffff) | ((((ptr[i]>>32)&0x2000) == 0x2000) ? 0xffffc000 : 0x0)));
         eventFrag->SetCharge(static_cast<Int_t>((ptr[i] >> 32) & 0xffff));
         //std::cout<<std::hex<<std::setfill('0');
         //std::cout<<std::setw(16)<<ptr[i]<<": addr "<<eventFrag->GetAddress()<<", ts "<<eventFrag->GetTimeStamp()<<", charge "<<eventFrag->Charge()<<std::endl;
         //std::cout<<std::dec<<std::setfill(' ');
         // we expect a second word from the channel+16 with the same timestamp
         ++i;
         if(i >= size / 8) {
            if(RecordDiag()) {
               TParsingDiagnostics::Get()->BadFragment(66);
            }
            Push(*BadOutputQueue(), std::make_shared<TBadFragment>(*eventFrag, reinterpret_cast<uint32_t*>(data), size / 4, i, false));
            continue;
         }
         // check address
         if(((ptr[i] >> 48) & 0xfff) != eventFrag->GetAddress() + 16) {
            //std::cout<<"Address mismatch "<<((ptr[i]>>48)&0xfff)<<" != "<<eventFrag->GetAddress()<<std::endl;
            if(RecordDiag()) {
               TParsingDiagnostics::Get()->BadFragment(67);
            }
            Push(*BadOutputQueue(), std::make_shared<TBadFragment>(*eventFrag, reinterpret_cast<uint32_t*>(data), size / 4, i, false));
            // re-try this word
            --i;
            continue;
         }
         // check timestamp
         if(static_cast<Long64_t>((ptr[i] & 0xfffffff) | (timeStampHighBits << 28)) != eventFrag->GetTimeStamp()) {
            //std::cout<<"Timestamp mismatch "<<((ptr[i]&0xfffffff) | (timeStampHighBits<<28))<<" != "<<eventFrag->GetTimeStamp()<<std::endl;
            if(RecordDiag()) {
               TParsingDiagnostics::Get()->BadFragment(68);
            }
            Push(*BadOutputQueue(), std::make_shared<TBadFragment>(*eventFrag, reinterpret_cast<uint32_t*>(data), size / 4, i, false));
            // re-try this word
            --i;
            continue;
         }
         DecrementInputSize();   // we just read another word from the input
         // set Cfd ???
         eventFrag->SetCfd(static_cast<Int_t>((ptr[i] >> 32) & 0xffff));
         //std::cout<<std::hex<<std::setfill('0');
         //std::cout<<std::setw(16)<<ptr[i]<<": addr "<<eventFrag->GetAddress()<<", ts "<<eventFrag->GetTimeStamp()<<", cfd "<<eventFrag->GetCfd()<<std::endl;
         //std::cout<<std::dec<<std::setfill(' ');
         if(RecordDiag()) {
            TParsingDiagnostics::Get()->GoodFragment(eventFrag);
         }
         Push(GoodOutputQueues(), std::make_shared<TFragment>(*eventFrag));
         break;
      }
      }
   }

   return totalEventsRead;
}
