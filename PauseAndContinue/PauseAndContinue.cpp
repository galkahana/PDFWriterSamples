// standard library includes
#include <iostream>
#include <string>
using namespace std;
// end standard library includes

// pdfwriter library includes
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"

using namespace PDFHummus;

static const string scBasePath =  "..\\PauseAndContinue\\Materials\\";
static const string scSystemFontsPath = "C:\\windows\\fonts\\";

int main(int argc, wchar_t* argv[])
{
	EStatusCode status;

	do
	{

		{
			PDFWriter pdfWriterA;
			
			status = pdfWriterA.StartPDF(scBasePath + "PauseAndContinue.pdf",ePDFVersion13);
			if(status != eSuccess)
				break;

			// Create a new page
			PDFPage* pdfPage = new PDFPage();
			pdfPage->SetMediaBox(PDFRectangle(0,0,595,842));

			// Create a content context for the page
			PageContentContext* pageContentContext = pdfWriterA.StartPageContentContext(pdfPage);

			PDFUsedFont* arialTTF = pdfWriterA.GetFontForFile(scSystemFontsPath + "arial.ttf");
			if(!arialTTF)
			{
				status = eFailure;
				break;
			}

			pageContentContext->k(0,0,0,1);

			pageContentContext->BT();
			pageContentContext->Tf(arialTTF,1);
			pageContentContext->Tm(20,0,0,20,40,822);
			pageContentContext->Tj("Hello World");
			pageContentContext->ET();				

			// End content context, and write the page
			status = pdfWriterA.EndPageContentContext(pageContentContext);
			if(status != eSuccess)
				break;

			status = pdfWriterA.WritePageAndRelease(pdfPage);
			if(status != eSuccess)
				break;

			status = pdfWriterA.Shutdown(scBasePath + "PauseAndContinueTest.txt");
			if(status != eSuccess)
				break;
		}
		

		{
			PDFWriter pdfWriterB;
			
			status = pdfWriterB.ContinuePDF(scBasePath + "PauseAndContinue.pdf",scBasePath + "PauseAndContinueTest.txt");
			if(status != eSuccess)
				break;

			// Create a new page
			PDFPage* pdfPage = new PDFPage();
			pdfPage->SetMediaBox(PDFRectangle(0,0,595,842));

			// Create a content context for the page
			PageContentContext* pageContentContext = pdfWriterB.StartPageContentContext(pdfPage);

			PDFUsedFont* arialTTF = pdfWriterB.GetFontForFile(scSystemFontsPath + "arial.ttf");
			if(!arialTTF)
			{
				status = eFailure;
				break;
			}

			pageContentContext->k(0,0,0,1);

			pageContentContext->BT();
			pageContentContext->Tf(arialTTF,1);
			pageContentContext->Tm(20,0,0,20,40,822);
			pageContentContext->Tj("Hello Again, World");
			pageContentContext->ET();				

			// End content context, and write the page
			status = pdfWriterB.EndPageContentContext(pageContentContext);
			if(status != eSuccess)
				break;

			status = pdfWriterB.WritePageAndRelease(pdfPage);
			if(status != eSuccess)
				break;

			status = pdfWriterB.EndPDF();
			if(status != eSuccess)
				break;
		}

	}while(false);
		
	if(eSuccess == status)
		cout<<"Succeeded in creating PauseAndContinue.PDF file\n";
	else
		cout<<"Failed in creating PauseAndContinue.PDF file\n";
		
	return 0;
}