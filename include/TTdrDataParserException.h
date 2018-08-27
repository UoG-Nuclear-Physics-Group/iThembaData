#ifndef TTDRDARAPARSEREXCEPTION_H
#define TTDRDARAPARSEREXCEPTION_H
/** \addtogroup Sorting
 *  @{
 */

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

#include <exception>
#include <string>

#include "TTdrDataParser.h"

class TTdrDataParserException : public std::exception {
public:
   TTdrDataParserException(TTdrDataParser::EDataParserState state, int failedWord, bool multipleErrors);
   ~TTdrDataParserException() override;

   const char* what() const noexcept override;

   int                                 GetFailedWord() { return fFailedWord; }
   TTdrDataParser::EDataParserState GetParserState() { return fParserState; }
   bool                                GetMultipleErrors() { return fMultipleErrors; }

private:
   TTdrDataParser::EDataParserState fParserState;
   int                                 fFailedWord;
   bool                                fMultipleErrors;
   std::string                         fMessage;
};
/*! @} */
#endif
