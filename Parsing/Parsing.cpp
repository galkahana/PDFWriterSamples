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

using namespace PDFHummus;

EStatusCode parsePDF();
void showPDFinfo(PDFParser&);
void showPagesInfo(PDFParser&,InputFile&,EStatusCode);
void checkXObjectRef(PDFParser&,RefCountPtr<PDFDictionary>);
void showXObjectsPerPageInfo(PDFParser&,PDFObjectCastPtr<PDFDictionary>);
void showPageContent(PDFParser&,RefCountPtr<PDFObject>,InputFile&);
void showContentStream(PDFStreamInput*,IByteReaderWithPosition*,PDFParser&);
 
static const string scBasePath =  "..\\Parsing\\Materials\\";
 
int main(int argc, char* argv[]) 
{  
    if(parsePDF() == eSuccess) 
		cout << "Parsing succeeded\n";
    else 
		cout << "Parsing failed\n";

    return 0;

}
 
EStatusCode parsePDF() 
{
	PDFParser parser;
	InputFile pdfFile;

	EStatusCode status = pdfFile.OpenFile(scBasePath + "XObjectContent.PDF");
	if(status != eSuccess) 
		return status;

	status = parser.StartPDFParsing(pdfFile.GetInputStream());
	if(status != eSuccess) 
		return status;

	showPDFinfo(parser); // Just wcout some info (no iteration)

	showPagesInfo(parser,pdfFile,status);

	return status;
}
 
void showPDFinfo(PDFParser& parser) 
{
    cout << "PDF Header level = " << parser.GetPDFLevel() << "\n";
    cout << "Number of objects in PDF = " << parser.GetObjectsCount() << "\n";
    cout << "Number of pages in PDF = " << parser.GetPagesCount() << "\n";
}
 
void showPagesInfo(PDFParser& parser, InputFile& pdfFile, EStatusCode status) 
{
    for(unsigned long i=0; i < parser.GetPagesCount() && eSuccess == status; ++i) 
	{
        cout << "Showing info for page " << i << ":\n";

        // Parse page object
        RefCountPtr<PDFDictionary> page(parser.ParsePage(i));

        // check XObject referenences
        checkXObjectRef(parser,page);

        // show page content
        RefCountPtr<PDFObject> contents(parser.QueryDictionaryObject(page.GetPtr(),"Contents"));
        if(!contents) 
		{
                cout << "No contents for this page\n";
                continue;
        }

        // content may be array or single
        cout << "Showing content of page:\n";
        showPageContent(parser,contents,pdfFile);
        
        cout << "End page content\n";
    }
}
 
void checkXObjectRef(PDFParser& parser,RefCountPtr<PDFDictionary> page) 
{
    PDFObjectCastPtr<PDFDictionary> resources(parser.QueryDictionaryObject(page.GetPtr(),"Resources"));
    if(!resources) 
	{
        wcout << "No XObject in this page\n";
        return;
    }

    PDFObjectCastPtr<PDFDictionary> xobjects(parser.QueryDictionaryObject(resources.GetPtr(),"XObject"));
    if(!xobjects) 
	{
		wcout << "No XObject in this page\n";
		return;
    }

    cout << "Displaying XObjects information for this page:\n";
    showXObjectsPerPageInfo(parser,xobjects);
}
 
void showXObjectsPerPageInfo(PDFParser& parser,PDFObjectCastPtr<PDFDictionary> xobjects) 
{
    RefCountPtr<PDFName> key;
    PDFObjectCastPtr<PDFIndirectObjectReference> value;
	MapIterator<PDFNameToPDFObjectMap> it = xobjects->GetIterator();
	while(it.MoveNext())
	{
        key = it.GetKey();
        value = it.GetValue();

        cout << "XObject named " << key->GetValue().c_str() << " is object " << value->mObjectID << " of type ";

        PDFObjectCastPtr<PDFStreamInput> xobject(parser.ParseNewObject(value->mObjectID));
        PDFObjectCastPtr<PDFDictionary> xobjectDictionary(xobject->QueryStreamDictionary());
        PDFObjectCastPtr<PDFName> typeOfXObject = xobjectDictionary->QueryDirectObject("Subtype");

        cout << typeOfXObject->GetValue().c_str() << "\n";
    }
}
 
void showPageContent(PDFParser& parser, RefCountPtr<PDFObject> contents, InputFile& pdfFile) 
{
    if(contents->GetType() == ePDFObjectArray) 
	{
        PDFObjectCastPtr<PDFIndirectObjectReference> streamReferences;
		SingleValueContainerIterator<PDFObjectVector> itContents = ((PDFArray*)contents.GetPtr())->GetIterator();
		// array of streams
		while(itContents.MoveNext())
		{
            streamReferences = itContents.GetItem();
            PDFObjectCastPtr<PDFStreamInput> stream = parser.ParseNewObject(streamReferences->mObjectID);
            showContentStream(stream.GetPtr(),pdfFile.GetInputStream(),parser);
        }
    } 
	else 
	{
        // stream
        showContentStream((PDFStreamInput*)contents.GetPtr(),pdfFile.GetInputStream(),parser);
    }
}
 
void showContentStream(PDFStreamInput* inStream,IByteReaderWithPosition* inPDFStream,PDFParser& inParser) 
{
    IByteReader* streamReader = inParser.CreateInputStreamReader(inStream);
    Byte buffer[1000];
    if(streamReader) 
	{
        inPDFStream->SetPosition(inStream->GetStreamContentStart());
        while(streamReader->NotEnded()) 
		{
            LongBufferSizeType readAmount = streamReader->Read(buffer,1000);
            cout.write((const char*)buffer,readAmount);
        }
        cout << "\n";
    } 
	else
		cout << "Unable to read content stream\n";
    delete streamReader;
}
