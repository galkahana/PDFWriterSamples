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
// end pdfwriter library includes

static const wstring scBasePath =  L"..\\Parsing\\Materials\\";

void ShowContentStream(PDFStreamInput* inStream,IByteReaderWithPosition* inPDFStream,PDFParser& inParser);

int wmain(int argc, wchar_t* argv[])
{
	EStatusCode status;
	
	do
	{
		PDFParser parser;
		InputFile pdfFile;

		status = pdfFile.OpenFile(scBasePath + L"XObjectContent.PDF");
		if(status != eSuccess)
			break;

		status = parser.StartPDFParsing(pdfFile.GetInputStream());
		if(status != eSuccess)
			break;
		
		wcout<<"PDF Header level = "<<parser.GetPDFLevel()<<"\n";
		wcout<<"Number of objects in PDF = "<<parser.GetObjectsCount()<<"\n";
		wcout<<"Number of pages in PDF = "<<parser.GetPagesCount()<<"\n";

		// show pages information
		for(unsigned long i=0; i < parser.GetPagesCount() && eSuccess == status;++i)
		{
			wcout<<"Showing info for page "<<i<<":\n";
			
			// Parse page object
			RefCountPtr<PDFDictionary> page(parser.ParsePage(i));

			// check XObject referenences
			do
			{
				PDFObjectCastPtr<PDFDictionary> resources(parser.QueryDictionaryObject(page.GetPtr(),"Resources"));
				if(!resources)
				{
					wcout<<"No XObject in this page\n";
					break;
				}

				PDFObjectCastPtr<PDFDictionary> xobjects(parser.QueryDictionaryObject(resources.GetPtr(),"XObject"));
				if(!xobjects)
				{
					wcout<<"No XObject in this page\n";
					break;
				}

				MapIterator<PDFNameToPDFObjectMap> it = xobjects->GetIterator();
				RefCountPtr<PDFName> key;
				PDFObjectCastPtr<PDFIndirectObjectReference> value;
				
				wcout<<"Displaying XObjects information for this page:\n";
				while(it.MoveNext())
				{
					key = it.GetKey();
					value = it.GetValue();

					wcout<<"XObject named "<<key->GetValue().c_str()<<" is object "<<value->mObjectID<<" of type ";

					PDFObjectCastPtr<PDFStreamInput> xobject(parser.ParseNewObject(value->mObjectID));
					PDFObjectCastPtr<PDFDictionary> xobjectDictionary(xobject->QueryStreamDictionary());
					PDFObjectCastPtr<PDFName> typeOfXObject = xobjectDictionary->QueryDirectObject("Subtype");

					wcout<<typeOfXObject->GetValue().c_str()<<"\n";
				}
			}while(false);

			// show page content
			RefCountPtr<PDFObject> contents(parser.QueryDictionaryObject(page.GetPtr(),"Contents"));
			if(!contents)
			{
				wcout<<"No contents for this page\n";
				continue;
			}

			// content may be array or single
			wcout<<"Showing content of page:\n";
			if(contents->GetType() == ePDFObjectArray)
			{
				// array of streams
				SingleValueContainerIterator<PDFObjectVector> itContents = ((PDFArray*)contents.GetPtr())->GetIterator();
				PDFObjectCastPtr<PDFIndirectObjectReference> streamReferences;
				while(itContents.MoveNext())
				{
					streamReferences = itContents.GetItem();
					PDFObjectCastPtr<PDFStreamInput> stream = parser.ParseNewObject(streamReferences->mObjectID);
					ShowContentStream(stream.GetPtr(),pdfFile.GetInputStream(),parser);
				}
			}
			else
			{
				// stream
				ShowContentStream((PDFStreamInput*)contents.GetPtr(),pdfFile.GetInputStream(),parser);
			}
			wcout<<"End page content\n";

		}
	}while(false);

	if(eSuccess == status)
		wcout<<"Parsing succeeded\n";
	else
		wcout<<"Parsing failed\n";

	return 0;
}

void ShowContentStream(PDFStreamInput* inStream,IByteReaderWithPosition* inPDFStream,PDFParser& inParser)
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
		wcout<<"\n";
	}
	else
		wcout<<"Unable to read content stream\n";
	delete streamReader;
}
