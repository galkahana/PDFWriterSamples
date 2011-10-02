// standard library includes
#include <iostream>
#include <string>
using namespace std;
// end standard library includes
 
// pdfwriter library includes
#include "PDFParser.h"
#include "InputFile.h"
#include "RefCountPtr.h"
#include "PDFDictionary.h"
#include "PDFArray.h"
#include "PDFObjectCast.h"
#include "PDFInteger.h"
#include "PDFReal.h"
#include "PDFName.h"
#include "PDFIndirectObjectReference.h"
#include "PDFStreamInput.h"
#include "IByteReader.h"
#include "EStatusCode.h"
// end pdfwriter library includes
 
PDFHummus::EStatusCode parsePDF();
void showPDFinfo(PDFParser);
void showPagesInfo(PDFParser,InputFile,PDFHummus::EStatusCode);
void checkXObjectRef(PDFParser,RefCountPtr<PDFDictionary>);
void showXObjectsPerPageInfo(PDFParser,PDFObjectCastPtr<PDFDictionary>);
void showPageContent(PDFParser,RefCountPtr<PDFObject>,InputFile);
void ShowContentStream(PDFStreamInput*,IByteReaderWithPosition*,PDFParser&);
 
static const wstring scBasePath =  L"..\\Parsing\\Materials\\";
 
int wmain(int argc, wchar_t* argv[]) {  
        if(parsePDF() == PDFHummus::eSuccess) wcout << "Parsing succeeded\n";
        else wcout << "Parsing failed\n";
 
        return 0;
}
 
PDFHummus::EStatusCode parsePDF() {
        PDFParser parser;
        InputFile pdfFile;
 
        PDFHummus::EStatusCode status = pdfFile.OpenFile(scBasePath + L"XObjectContent.PDF");
        if(status != PDFHummus::eSuccess) return status;
 
        status = parser.StartPDFParsing(pdfFile.GetInputStream());
        if(status != PDFHummus::eSuccess) return status;
 
        showPDFinfo(parser); // Just wcout some info (no iteration)
 
        showPagesInfo(parser,pdfFile,status);
}
 
void showPDFinfo(PDFParser parser) {
        wcout << "PDF Header level = " << parser.GetPDFLevel() << "\n";
        wcout << "Number of objects in PDF = " << parser.GetObjectsCount() << "\n";
        wcout << "Number of pages in PDF = " << parser.GetPagesCount() << "\n";
}
 
void showPagesInfo(PDFParser parser, InputFile pdfFile, PDFHummus::EStatusCode status) {
        for(unsigned long i=0; i < parser.GetPagesCount() && status==PDFHummus::eSuccess; ++i) {
                wcout << "Showing info for page " << i << ":\n";
 
                // Parse page object
                RefCountPtr<PDFDictionary> page(parser.ParsePage(i));
 
                // check XObject referenences
                checkXObjectRef(parser,page);
 
                // show page content
                RefCountPtr<PDFObject> contents(parser.QueryDictionaryObject(page.GetPtr(),"Contents"));
                if(!contents) {
                        wcout << "No contents for this page\n";
                        continue;
                }
 
                // content may be array or single
                wcout << "Showing content of page:\n";
                showPageContent(parser,contents,pdfFile);
                
                wcout << "End page content\n";
        }
}
 
void checkXObjectRef(PDFParser parser,RefCountPtr<PDFDictionary> page) {
        PDFObjectCastPtr<PDFDictionary> resources(parser.QueryDictionaryObject(page.GetPtr(),"Resources"));
        if(!resources) {
                wcout << "No XObject in this page\n";
                return;
        }
 
        PDFObjectCastPtr<PDFDictionary> xobjects(parser.QueryDictionaryObject(resources.GetPtr(),"XObject"));
        if(!xobjects) {
                wcout << "No XObject in this page\n";
                return;
        }
 
        wcout << "Displaying XObjects information for this page:\n";
        showXObjectsPerPageInfo(parser,xobjects);
}
 
void showXObjectsPerPageInfo(PDFObjectCastPtr<PDFDictionary> xobjects, PDFParser parser) {
        RefCountPtr<PDFName> key;
        PDFObjectCastPtr<PDFIndirectObjectReference> value;
        for(MapIterator<PDFNameToPDFObjectMap> it = xobjects->GetIterator(); ;it.MoveNext()) {
                key = it.GetKey();
                value = it.GetValue();
 
                wcout << "XObject named " << key->GetValue().c_str() << " is object " << value->mObjectID << " of type ";
 
                PDFObjectCastPtr<PDFStreamInput> xobject(parser.ParseNewObject(value->mObjectID));
                PDFObjectCastPtr<PDFDictionary> xobjectDictionary(xobject->QueryStreamDictionary());
                PDFObjectCastPtr<PDFName> typeOfXObject = xobjectDictionary->QueryDirectObject("Subtype");
 
                wcout << typeOfXObject->GetValue().c_str() << "\n";
        }
}
 
void showPageContent(PDFParser parser, RefCountPtr<PDFObject> contents, InputFile pdfFile) {
        if(contents->GetType() == ePDFObjectArray) {
                PDFObjectCastPtr<PDFIndirectObjectReference> streamReferences;
                        for(SingleValueContainerIterator<PDFObjectVector> itContents = ((PDFArray*)contents.GetPtr())->GetIterator(); /*<-- an array of streams*/; itContents.MoveNext()) {
                                streamReferences = itContents.GetItem();
                                PDFObjectCastPtr<PDFStreamInput> stream = parser.ParseNewObject(streamReferences->mObjectID);
                                ShowContentStream(stream.GetPtr(),pdfFile.GetInputStream(),parser);
                        }
        } else {
                // stream
                ShowContentStream((PDFStreamInput*)contents.GetPtr(),pdfFile.GetInputStream(),parser);
        }
}
 
void ShowContentStream(PDFStreamInput* inStream,IByteReaderWithPosition* inPDFStream,PDFParser& inParser) {
        IByteReader* streamReader = inParser.CreateInputStreamReader(inStream);
        Byte buffer[1000];
        if(streamReader) {
                inPDFStream->SetPosition(inStream->GetStreamContentStart());
                while(streamReader->NotEnded()) {
                        LongBufferSizeType readAmount = streamReader->Read(buffer,1000);
                        cout.write((const char*)buffer,readAmount);
                }
                wcout << "\n";
        } else
                wcout << "Unable to read content stream\n";
        delete streamReader;
}